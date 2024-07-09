#include "carplang.h"

#include "carpassert.h"

#include <stdlib.h> // calloc
#include <string.h> // memcpy

typedef enum CarpTokenType
{
    CarpTokenTypeUnknownToken,
    CarpTokenTypeEndOfTokens, // '\0'
    CarpTokenTypePercentSign, // %
    CarpTokenTypePlus, // +
    CarpTokenTypeMinus, // -
    CarpTokenTypeStar, // *
    CarpTokenTypeSlash, // /
    CarpTokenTypeBackSlash, // '\'
    CarpTokenTypeComma, // ,
    CarpTokenTypeDot, // .
    CarpTokenTypeF, // 'f'
    CarpTokenTypeComment, // //
    CarpTokenTypeCommentBlockBegin, // '*/'

    CarpTokenTypeSetEqual, // :=
    CarpTokenTypeNotEqual, // !=
    CarpTokenTypeDoubleEqual, // ==
    CarpTokenTypeDoubleQuote, // "
    CarpTokenTypeExclamationMark, // !
    CarpTokenTypeAtSign, // @
    CarpTokenTypeBitwiseAnd, // &
    CarpTokenTypeBitwiseOr, // |
    CarpTokenTypeBitwiseXor, // ^
    CarpTokenTypeBitwiseNot, //~
    CarpTokenTypeLogicalAnd, // ||
    CarpTokenTypeLogicalOr, // ||
    CarpTokenTypeHashSign, // #
    CarpTokenTypeQuestionMark, // ?
    CarpTokenTypeColon, // :
    CarpTokenTypeSemiColon, // ;
    CarpTokenTypeParenthesesOpen, // ()
    CarpTokenTypeParenthesesClose,
    CarpTokenTypeSquareBracketOpen, // []
    CarpTokenTypeSquareBracketClose,
    CarpTokenTypeCurlyBracketOpen, // {}
    CarpTokenTypeCurlyBracketClose,
    CarpTokenTypeAngleBracketOpen, // <>
    CarpTokenTypeAngleBracketClose,

    // String type
    CarpTokenTypeString,

    // Number type
    CarpTokenTypeNumber,

    // Keywords
    CarpTokenTypeConst,
    CarpTokenTypeWhile,
    CarpTokenTypeFunc,
    CarpTokenTypeLet,
    CarpTokenTypeVar,

    CarpTokenTypeIdentifier,

    CarpTokenTypeCount,
} CarpTokenType;


typedef struct CarpToken
{
    const u8* carpToken;
    s32 carpTokenLen;
    CarpTokenType carpTokenType;
} CarpToken;

static_assert((s32)(sizeof(CarpToken) & (sizeof(CarpToken) - 1)) == 0, "CarpToken size not power of 2");

typedef struct CarpTokenHelper
{
    const char* carpTokenHelperStr;
    s32 carpTokenHelperLen;
    CarpTokenType carpTokenHelperType;
} CarpTokenHelper;

#define CarpTokenHelperLine(name, type) { name, (sizeof(name) - 1), type }

static const CarpTokenHelper CarpTokenHelpers[] =
{
    CarpTokenHelperLine("\0", CarpTokenTypeEndOfTokens), // '\0'
    CarpTokenHelperLine(":=", CarpTokenTypeSetEqual), // :=
    CarpTokenHelperLine("!=", CarpTokenTypeNotEqual), // !=
    CarpTokenHelperLine("==", CarpTokenTypeDoubleEqual), // ==
    CarpTokenHelperLine("\"", CarpTokenTypeDoubleQuote), // "
    CarpTokenHelperLine("||", CarpTokenTypeLogicalOr), // '\'
    CarpTokenHelperLine("&&", CarpTokenTypeLogicalAnd), // '\'
    CarpTokenHelperLine("//", CarpTokenTypeComment), // /
    CarpTokenHelperLine("\\", CarpTokenTypeBackSlash), // '\'
    CarpTokenHelperLine("/*", CarpTokenTypeCommentBlockBegin), // '*/'

    CarpTokenHelperLine("%", CarpTokenTypePercentSign), // %
    CarpTokenHelperLine("+", CarpTokenTypePlus), // +
    CarpTokenHelperLine("-", CarpTokenTypeMinus), // -
    CarpTokenHelperLine("*", CarpTokenTypeStar), // *
    CarpTokenHelperLine("/", CarpTokenTypeSlash), // /
    CarpTokenHelperLine(",", CarpTokenTypeComma), // ,
    CarpTokenHelperLine(".", CarpTokenTypeDot), // .
    CarpTokenHelperLine("f", CarpTokenTypeF), // 'f'
    CarpTokenHelperLine("!", CarpTokenTypeExclamationMark), // !
    CarpTokenHelperLine("@", CarpTokenTypeAtSign), // @
    CarpTokenHelperLine("&", CarpTokenTypeBitwiseAnd), // &
    CarpTokenHelperLine("|", CarpTokenTypeBitwiseOr), // |
    CarpTokenHelperLine("^", CarpTokenTypeBitwiseXor), // ^
    CarpTokenHelperLine("^", CarpTokenTypeBitwiseNot), // ~
    CarpTokenHelperLine("#", CarpTokenTypeHashSign), // #
    CarpTokenHelperLine("?", CarpTokenTypeQuestionMark), // ?

    CarpTokenHelperLine(":", CarpTokenTypeColon), // :
    CarpTokenHelperLine(";", CarpTokenTypeSemiColon), // ;

    CarpTokenHelperLine("(", CarpTokenTypeParenthesesOpen), // ()
    CarpTokenHelperLine(")", CarpTokenTypeParenthesesClose),
    CarpTokenHelperLine("[", CarpTokenTypeSquareBracketOpen), // []
    CarpTokenHelperLine("]", CarpTokenTypeSquareBracketClose),
    CarpTokenHelperLine("{", CarpTokenTypeCurlyBracketOpen), // {}
    CarpTokenHelperLine("}", CarpTokenTypeCurlyBracketClose),
    CarpTokenHelperLine("<", CarpTokenTypeAngleBracketOpen), // <>
    CarpTokenHelperLine(">", CarpTokenTypeAngleBracketClose),

    CarpTokenHelperLine("const", CarpTokenTypeConst),
    CarpTokenHelperLine("while", CarpTokenTypeWhile),
    CarpTokenHelperLine("fn", CarpTokenTypeFunc),
    CarpTokenHelperLine("let", CarpTokenTypeLet),
    CarpTokenHelperLine("var", CarpTokenTypeVar),
};
static const s32 CarpTokenHelperCount
    = (s32)(sizeof(CarpTokenHelpers) / sizeof(CarpTokenHelper));


#undef CarpTokenHelperLine

typedef struct CarpTokenIndex
{
    const CarpBuffer tokenIndexBuffer;
    s32 tokenIndexIndex;
} CarpTokenIndex;

static bool sSkipWhiteSpace(CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, false);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, false);

    while(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        switch(tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex])
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return true;
        }
        ++tokenIndex->tokenIndexIndex;
    }
    return false;
}


static s32 sTryParseIdentifier(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, -1);

    char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex];
    if(!(
        (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || c == '_'))
    {
        return 0;
    }
    s32 len = 1;

    while(tokenIndex->tokenIndexIndex + len < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex + len];
        if(!(
            (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || c == '_'))
        {
            return len;
        }
        ++len;
    }
    return -1;
}

static s32 sTryParseNumber(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, -1);

    s32 len = 0;

    while(tokenIndex->tokenIndexIndex + len < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex + len];
        if(c < '0' || c > '9')
        {
            return len;
        }
        ++len;
    }
    return -1;
}

static bool sPushToken(const CarpToken* token, CarpBuffer *outTokenBuffer)
{
    return carp_buffer_pushBuffer(outTokenBuffer, (const u8*)token, sizeof(CarpToken));
}

static bool sParseTokens(CarpTokenIndex* tokenIndex, CarpBuffer *outTokenBuffer)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(outTokenBuffer, false);

    const CarpBuffer* input = &(tokenIndex->tokenIndexBuffer);
    CARP_ASSERT_RETURN(input->carpBufferSize > 0, false);
    CARP_ASSERT_RETURN(input->carpBufferData, false);
    CARP_ASSERT_RETURN(input->carpBufferData[input->carpBufferSize - 1] == '\0', false);

    CARP_ASSERT_RETURN(carp_buffer_create((1 << 20), 16, outTokenBuffer), false);

    const u8* pos = input->carpBufferData;

    while(tokenIndex->tokenIndexIndex < input->carpBufferSize)
    {
        CarpToken pushToken = { 0 };
        tokenIndex->tokenIndexIndex = pos - input->carpBufferData;

        CARP_ASSERT_RETURN(sSkipWhiteSpace(tokenIndex), false);
        CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < input->carpBufferSize, false);
        pos = input->carpBufferData + tokenIndex->tokenIndexIndex;
        pushToken.carpToken = pos;

        // Print start of token
        //CARP_LOG("W: %-.*s\n", 4, pos);

        s32 idLen = sTryParseIdentifier(tokenIndex);
        CARP_ASSERT_RETURN(idLen >= 0, false);


        if(idLen > 0)
        {
            pushToken.carpTokenType = CarpTokenTypeIdentifier;
            pushToken.carpTokenLen = idLen;
        }

        for(s32 helperIndex = 0; helperIndex < CarpTokenHelperCount; ++helperIndex)
        {
            const CarpTokenHelper* helper = CarpTokenHelpers + helperIndex;
            if(idLen <=  helper->carpTokenHelperLen
                && memcmp(helper->carpTokenHelperStr, pos, helper->carpTokenHelperLen) == 0)
            {
                idLen = helper->carpTokenHelperLen;

                pushToken.carpTokenType = helper->carpTokenHelperType;
                pushToken.carpTokenLen = helper->carpTokenHelperLen;

                if(helper->carpTokenHelperType == CarpTokenTypeEndOfTokens)
                {
                    CARP_ASSERT_RETURN(sPushToken(&pushToken, outTokenBuffer), false);
                    return true;
                }
                else if(helper->carpTokenHelperType == CarpTokenTypeDoubleQuote)
                {
                    const u8* prev = pos;
                    ++pos;
                    while(tokenIndex->tokenIndexIndex < input->carpBufferSize
                        && *pos != '\0' && ( *pos != '\"' || *prev == '\\' ))
                    {
                        ++prev;
                        ++pos;
                    }
                    CARP_ASSERT_RETURN(*pos == '\"', false);
                    ++pos;
                }
                else if(helper->carpTokenHelperType == CarpTokenTypeComment)
                {
                    while(tokenIndex->tokenIndexIndex < input->carpBufferSize
                        && *pos != '\0' && *pos != '\n')
                    {
                        ++pos;
                    }
                }
                else if(helper->carpTokenHelperType == CarpTokenTypeCommentBlockBegin)
                {
                    while(tokenIndex->tokenIndexIndex < input->carpBufferSize
                        && *pos != '\0' && !(*pos == '*' && *(pos + 1) == '/'))
                    {
                        ++pos;
                    }
                    CARP_ASSERT_RETURN(*pos != '\0', false);
                    pos += 2;
                }
                break;
            }
        }
        if(pushToken.carpTokenType == CarpTokenTypeComment
            || pushToken.carpTokenType == CarpTokenTypeCommentBlockBegin)
        {
            continue;
        }

        else if(pushToken.carpTokenType == CarpTokenTypeDoubleQuote)
        {
            pushToken.carpTokenType = CarpTokenTypeString;
            pushToken.carpTokenLen = pos - pushToken.carpToken;
        }
        else if(pushToken.carpTokenType == CarpTokenTypeUnknownToken)
        {
            s32 numLen = sTryParseNumber(tokenIndex);
            CARP_ASSERT_RETURN(numLen > 0, false);

            pushToken.carpTokenLen = numLen;
            pushToken.carpTokenType = CarpTokenTypeNumber;
            pos += numLen;
        }
        else 
        {
            pos += idLen;
        }
        CARP_ASSERT_RETURN(pushToken.carpTokenType != CarpTokenTypeUnknownToken, false);
        CARP_ASSERT_RETURN(sPushToken(&pushToken, outTokenBuffer), false);
    }

    return false;
}
void sPrintTokenErrorAt(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, ;);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, ;);

    s32 lineNumber = 0;
    s32 charNumber = 0;

    s32 index = 0;
    s32 nextLineIndex = 0;
    while(index < tokenIndex->tokenIndexIndex)
    {
        char c = (char)tokenIndex->tokenIndexBuffer.carpBufferData[index];
        if(c == '\n')
        {
            lineNumber++;
            charNumber = 0;
        }
        ++charNumber;
        ++index;
    }
    nextLineIndex = index + 1;
    while(nextLineIndex < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        char c = (char)tokenIndex->tokenIndexBuffer.carpBufferData[nextLineIndex];
        if(c == '\n' || c == '\0')
        {
            break;
        }
        ++nextLineIndex;
    }


    s32 printLenL = (tokenIndex->tokenIndexIndex) > 5 ? 5 : tokenIndex->tokenIndexIndex;
    s32 printLenR = nextLineIndex - tokenIndex->tokenIndexIndex;
    s32 printLen = printLenL +  printLenR;
    index = tokenIndex->tokenIndexIndex - printLenL;

    CARP_LOGERROR("Error at: %i:%i \"%-.*s\"\n",
        lineNumber,
        charNumber,
        printLen,
        tokenIndex->tokenIndexBuffer.carpBufferData + index);

}
void sPrintTokens(const CarpBuffer* input, const CarpBuffer* tokenBuffer)
{
    const CarpToken* token = (const CarpToken*)tokenBuffer->carpBufferData;
    const CarpToken* endToken = (const CarpToken*)
        (tokenBuffer->carpBufferData + tokenBuffer->carpBufferSize);
    s32 tokenCount = 0;
    while(token < endToken)
    {
        const char* name = "";
        switch(token->carpTokenType)
        {
            case CarpTokenTypeUnknownToken: name = "Unknown"; break;
            case CarpTokenTypeEndOfTokens: name = "End Of CarpTokens"; break;
            case CarpTokenTypePercentSign: name = "%"; break;
            case CarpTokenTypePlus: name = "+"; break;
            case CarpTokenTypeMinus: name = "-"; break;
            case CarpTokenTypeStar: name = "*"; break;
            case CarpTokenTypeSlash: name = "/"; break;
            case CarpTokenTypeBackSlash: name = "\\"; break;
            case CarpTokenTypeComma: name = ","; break;
            case CarpTokenTypeDot: name = "."; break;
            case CarpTokenTypeF: name = "f"; break;
            case CarpTokenTypeSetEqual: name = ":="; break;
            case CarpTokenTypeNotEqual: name = "!="; break;
            case CarpTokenTypeDoubleEqual: name = "=="; break;
            case CarpTokenTypeDoubleQuote: name = "\""; break;
            case CarpTokenTypeExclamationMark: name = "!"; break;
            case CarpTokenTypeAtSign: name = "@"; break;
            case CarpTokenTypeBitwiseAnd: name = "&"; break;
            case CarpTokenTypeBitwiseOr: name = "|"; break;
            case CarpTokenTypeBitwiseXor: name = "^"; break;
            case CarpTokenTypeBitwiseNot: name = "~"; break;
            case CarpTokenTypeLogicalAnd: name = "&&"; break;
            case CarpTokenTypeLogicalOr: name = "||"; break;
            case CarpTokenTypeHashSign: name = "#"; break;
            case CarpTokenTypeQuestionMark: name = "?"; break;
            case CarpTokenTypeColon: name = ":"; break;
            case CarpTokenTypeSemiColon: name = ";"; break;
            case CarpTokenTypeParenthesesOpen: name = "("; break;
            case CarpTokenTypeParenthesesClose: name = ")"; break;
            case CarpTokenTypeSquareBracketOpen: name = "["; break;
            case CarpTokenTypeSquareBracketClose: name = "]"; break;
            case CarpTokenTypeCurlyBracketOpen: name = "{"; break;
            case CarpTokenTypeCurlyBracketClose: name = "}"; break;
            case CarpTokenTypeAngleBracketOpen: name = "<"; break;
            case CarpTokenTypeAngleBracketClose: name = ">"; break;

            case CarpTokenTypeComment: name = "//"; break;
            case CarpTokenTypeCommentBlockBegin: name = "/*"; break;

            // String type
            case CarpTokenTypeString: name = ""; break;

            // Number type
            case CarpTokenTypeNumber: name = "Num"; break;

            // Keywords
            case CarpTokenTypeConst: name = "Const"; break;
            case CarpTokenTypeWhile: name = "While"; break;
            case CarpTokenTypeFunc: name = "Func"; break;
            case CarpTokenTypeLet: name = "Let"; break;
            case CarpTokenTypeVar: name = "Var"; break;

            case CarpTokenTypeIdentifier: name = "Identifier"; break;
            case CarpTokenTypeCount: name = "Count"; break;
        }
        if((tokenCount % 8) == 0)
        {
            CARP_LOG("\n");
        }
        if(token->carpTokenType == CarpTokenTypeIdentifier
            || token->carpTokenType == CarpTokenTypeNumber
            || token->carpTokenType == CarpTokenTypeString)
        {
            CARP_LOG("'%s[%-.*s]', ", name, token->carpTokenLen, token->carpToken);
        }
        else
        {
            CARP_LOG("'%s', ", name);
        }
        ++token;
        ++tokenCount;
    }
    CARP_LOG("\n");
}















//////////////////////
//
// AST
//
/////////////////////

typedef enum CarpExprType
{
    CarpExprTypeAdd,
    CarpExprTypeSub,
    CarpExprTypeMul,
    CarpExprTypeDiv,


    CarpExprTypeIntNumber,
    CarpExprTypeRealNumber,
    CarpExprString,

    CarpExprTypeCount,

} CarpExprType;

typedef struct CarpExpr
{
    union
    {
        struct
        {
            int32_t carpASTNodeStart;
        };
        s64 intNumber;
        f64 realNumber;
    };
    int32_t carpASTNodeLen;
    CarpExprType carpASTNodeType;
} CarpExpr;


typedef struct CarpASTNode
{
    CarpExpr* left;
    CarpExpr* right;
    CarpExprType carpASTNodeType;
} CarpASTNode;

typedef struct CarpTokenIndexer
{
    const CarpToken* carpTokenIndexerTokens;
    s32 carpTokenIndexerTokenAmount;
    s32 carpTokenIndexerIndex;
} CarpTokenIndexer;

static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(tokenIndexer->carpTokenIndexerIndex < tokenIndexer->carpTokenIndexerTokenAmount, false);
    ++tokenIndexer->carpTokenIndexerIndex;
    return true;
}

static bool sGetNextToken(const CarpTokenIndexer* tokenIndexer, const CarpToken** outToken)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outToken, false);
    CARP_ASSERT_RETURN(tokenIndexer->carpTokenIndexerIndex < tokenIndexer->carpTokenIndexerTokenAmount, false);

    *outToken = tokenIndexer->carpTokenIndexerTokens + tokenIndexer->carpTokenIndexerIndex;
    return true;
}


static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetNextToken(tokenIndexer, &next), false);
    if(next->carpTokenType == type)
        sAdvanceToken(tokenIndexer);
    return next->carpTokenType == type;
}

static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -2);
    CARP_ASSERT_RETURN(outASTBuffer, -2);

    s32 currentIndex = tokenIndexer->carpTokenIndexerIndex;

    bool neg = sAdvanceIfToken(tokenIndexer, CarpTokenTypeMinus);

    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetNextToken(tokenIndexer, &next), false);
    char* endPtr = NULL;

    if(!sAdvanceIfToken(tokenIndexer, CarpTokenTypeNumber))
    {
        if(neg)
            tokenIndexer->carpTokenIndexerIndex--;
        return -1;
    }
    CarpExpr expr = {0};

    s32 result = outASTBuffer->carpBufferSize;

    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeDot))
    {
        const CarpToken* nextDecimal = NULL;
        CARP_ASSERT_RETURN(sGetNextToken(tokenIndexer, &nextDecimal), false);

        expr.realNumber = strtod((const char*)next->carpToken, &endPtr);
        s32 len = (s32)(endPtr - (const char*)next->carpToken);
        printf("%i vs %i\n", len, next->carpTokenLen + 1 + nextDecimal->carpTokenLen);
        CARP_ASSERT_RETURN(len == next->carpTokenLen + 1 + nextDecimal->carpTokenLen, -2);
        expr.carpASTNodeType = CarpExprTypeRealNumber;
        if(neg)
            expr.realNumber = -expr.realNumber;
        CARP_LOG("float: %f\n", expr.realNumber);
    }
    else
    {
        expr.intNumber = strtoll((const char*)next->carpToken, &endPtr, 10);
        expr.carpASTNodeType = CarpExprTypeIntNumber;
        if(neg)
            expr.intNumber = -expr.intNumber;
        CARP_LOG("s64: %" PRIi64 "\n", expr.intNumber);
    }
    CARP_ASSERT_RETURN(carp_buffer_pushBuffer(outASTBuffer, (const u8*)&expr, sizeof(CarpExpr)), -2);

    return result;
}

static bool sParseExpression(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);
    s32 numberResult = sParseNumber(tokenIndexer, outASTBuffer);
    CARP_ASSERT_RETURN(numberResult >= -1, false); 
    return numberResult >= 0;
}

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);

    return sParseExpression(tokenIndexer, outASTBuffer);
}

static bool sParseAST(const CarpBuffer* tokenBuffer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenBuffer, false);
    CARP_ASSERT_RETURN(tokenBuffer->carpBufferData, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);
    CARP_ASSERT_RETURN(outASTBuffer->carpBufferData == NULL, false);

    CARP_ASSERT_RETURN(carp_buffer_create((1 << 20), 16, outASTBuffer), false);


    const CarpToken* tokens = (const CarpToken*)tokenBuffer->carpBufferData;
    CarpTokenIndexer indexer = {0};

    indexer.carpTokenIndexerTokens = tokens;
    indexer.carpTokenIndexerTokenAmount = tokenBuffer->carpBufferSize / sizeof(CarpToken);

    while(indexer.carpTokenIndexerIndex < indexer.carpTokenIndexerTokenAmount)
    {
        sParseStatement(&indexer, outASTBuffer);
        indexer.carpTokenIndexerIndex++;
        /*
        if(!sParseStatement())
        {
            CARP_LOGERROR("Failed to parse ast\n");
            return false;
        }
        */
    }

    return true;
}

CARP_FN bool carp_lang_compileToSpecialBuffer(
    const char* input,
    s32 inputLen,
    CarpSpecialBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(input, false);
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(inputLen > 0, false);

    CarpBuffer tokenOutBuffer = {0};
    const CarpBuffer inputBuffer = {
            .carpBufferData = (u8*)input,
            .carpBufferCapacity = inputLen,
            .carpBufferSize = inputLen,
            .carpBufferAlignment = 1,
    };

    CarpTokenIndex tokenIndex = {.tokenIndexBuffer = inputBuffer };
    if(!sParseTokens(&tokenIndex, &tokenOutBuffer))
    {
        sPrintTokenErrorAt(&tokenIndex);
        CARP_LOGERROR("Failed to parse tokens\n");
        return false;
    }
    sPrintTokens(&tokenIndex.tokenIndexBuffer, &tokenOutBuffer);

    /*
    for(int i = 0; i < CarpTokenHelperCount; ++i)
    {
        CARP_LOG("%i, str: %s, len:%i, type: %i\n", 
            i, 
            CarpTokenHelpers[i].carpTokenHelperStr,
            CarpTokenHelpers[i].carpTokenHelperLen,
            CarpTokenHelpers[i].carpTokenHelperType);
    }
    */

    CarpBuffer commandBuffer = {0};
    if(!sParseAST(&tokenOutBuffer, &commandBuffer))
    {
        CARP_LOGERROR("Failed to parse commands\n");
        return false;
    }

    carp_buffer_free(&tokenOutBuffer);
    carp_buffer_free(&commandBuffer);
    return true;
}
