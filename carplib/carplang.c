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
    s32 carpTokenStart;
    s32 carpTokenLen;
    CarpTokenType carpTokenType;
} CarpToken;


typedef struct CarpTokenHelper
{
    const char* tokenHelperStr;
    s32 tokenHelperLen;
    CarpTokenType tokenHelperType;
} CarpTokenHelper;

#define CarpTokenHelperLine(name, type) { name, (sizeof(name) - 1), type }

static const CarpTokenHelper tokenHelpers[] =
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
    CarpTokenHelperLine("|", CarpTokenTypeBitwiseOr), // &
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
    = (s32)(sizeof(tokenHelpers) / sizeof(CarpTokenHelper));


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

    s32 capacity = (1 << 18) * (sizeof(CarpToken) + 4);
    CARP_ASSERT_RETURN(carp_buffer_create(capacity, 16, outTokenBuffer), false);

    while(tokenIndex->tokenIndexIndex < input->carpBufferSize)
    {
        CarpToken pushToken = { 0 };

        CARP_ASSERT_RETURN(sSkipWhiteSpace(tokenIndex), false);
        CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < input->carpBufferSize, false);

        pushToken.carpTokenStart = tokenIndex->tokenIndexIndex;
        const u8* pos = input->carpBufferData + tokenIndex->tokenIndexIndex;

        s32 idLen = sTryParseIdentifier(tokenIndex);
        CARP_ASSERT_RETURN(idLen >= 0, false);

        if(idLen > 0)
        {
            pushToken.carpTokenType = CarpTokenTypeIdentifier;
            pushToken.carpTokenLen = idLen;
        }

        for(s32 helperIndex = 0; helperIndex < CarpTokenHelperCount; ++helperIndex)
        {
            const CarpTokenHelper* helper = tokenHelpers + helperIndex;
            if(idLen <=  helper->tokenHelperLen
                && memcmp(helper->tokenHelperStr, pos, helper->tokenHelperLen) == 0)
            {
                pushToken.carpTokenType = helper->tokenHelperType;
                pushToken.carpTokenLen = helper->tokenHelperLen;

                if(helper->tokenHelperType == CarpTokenTypeEndOfTokens)
                {
                    CARP_ASSERT_RETURN(sPushToken(&pushToken, outTokenBuffer), false);
                    return true;
                }
                else if(helper->tokenHelperType == CarpTokenTypeComment)
                {
                    while(tokenIndex->tokenIndexIndex < input->carpBufferSize
                        && input->carpBufferData[tokenIndex->tokenIndexIndex] != '\0'
                        && input->carpBufferData[tokenIndex->tokenIndexIndex] != '\n')
                    {
                        ++(tokenIndex->tokenIndexIndex);
                    }
                }
                else if(helper->tokenHelperType == CarpTokenTypeCommentBlockBegin)
                {
                    while(tokenIndex->tokenIndexIndex < input->carpBufferSize
                        && input->carpBufferData[tokenIndex->tokenIndexIndex] != '\0'
                        && memcmp( input->carpBufferData + tokenIndex->tokenIndexIndex, "*/", 2) != 0)
                    {
                        ++(tokenIndex->tokenIndexIndex);
                    }
                    CARP_ASSERT_RETURN(input->carpBufferData[tokenIndex->tokenIndexIndex] != '\0', false);
                    (tokenIndex->tokenIndexIndex) += 2;
                }
                break;
            }
        }
        if(pushToken.carpTokenType == CarpTokenTypeComment
            || pushToken.carpTokenType == CarpTokenTypeCommentBlockBegin)
        {
            continue;
        }
        if(pushToken.carpTokenType == CarpTokenTypeUnknownToken)
        {
            s32 numLen = sTryParseNumber(tokenIndex);
            CARP_ASSERT_RETURN(numLen > 0, false);

            pushToken.carpTokenLen = numLen;
            pushToken.carpTokenType = CarpTokenTypeNumber;
        }
        CARP_ASSERT_RETURN(pushToken.carpTokenType != CarpTokenTypeUnknownToken, false);

        tokenIndex->tokenIndexIndex += pushToken.carpTokenLen;
        CARP_ASSERT_RETURN(sPushToken(&pushToken, outTokenBuffer), false);
    }

    return false;
}
void sPrintErrorAt(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, ;);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, ;);

    s32 lineNumber = 0;
    s32 charNumber = 0;

    s32 index = 0;
    while(index < tokenIndex->tokenIndexIndex)
    {
        char c = (char)tokenIndex->tokenIndexBuffer.carpBufferData[index];
        if(c == '\n')
        {
            lineNumber++;
            charNumber = 0;
        }
        ++index;
    }

    CARP_LOGERROR("Error at: %i:%i \"%.5s\"\n",
        lineNumber,
        charNumber,
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

            // Number type
            case CarpTokenTypeNumber: name = "Number"; break;

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
            || token->carpTokenType == CarpTokenTypeNumber)
        {
            CARP_LOG("'%s[%-.*s]', ",
                name,
                token->carpTokenLen,
                (const char*)(input->carpBufferData + token->carpTokenStart));

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

typedef enum CarpExprType {
    CarpExprTypeAdd,
    CarpExprTypeSub,
    CarpExprTypeMul,
    CarpExprTypeDiv,


    CarpExprTypeIntNumber,
    CarpExprTypeFloatNumber,
    CarpExprString,

    CarpExprTypeCount,

} CarpExprType;

typedef struct CarpExpr {
    CarpExprType carpASTNodeType;
    int32_t carpASTNodeStart;
    int32_t carpASTNodeLen;
} CarpExpr;


typedef struct CarpASTNode {
    CarpExprType carpASTNodeType;
    CarpExpr* left;
    CarpExpr* right;
} CarpASTNode;


static bool sParseNumber(const CarpBuffer* tokenBuffer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenBuffer, false);
    CARP_ASSERT_RETURN(tokenBuffer->carpBufferData, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);
    CARP_ASSERT_RETURN(outASTBuffer->carpBufferData == NULL, false);

    const CarpToken* tokens = (const CarpToken*)tokenBuffer->carpBufferData;

    if(tokens->carpTokenType == CarpTokenTypeNumber)
    {

    }

    return false;
}

static bool sParseExpression(const CarpBuffer* tokenBuffer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenBuffer, false);
    CARP_ASSERT_RETURN(tokenBuffer->carpBufferData, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);
    CARP_ASSERT_RETURN(outASTBuffer->carpBufferData == NULL, false);

    const CarpToken* tokens = (const CarpToken*)tokenBuffer->carpBufferData;

    return true;
}

static bool sParseAST(const CarpBuffer* tokenBuffer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenBuffer, false);
    CARP_ASSERT_RETURN(tokenBuffer->carpBufferData, false);
    CARP_ASSERT_RETURN(outASTBuffer, false);
    CARP_ASSERT_RETURN(outASTBuffer->carpBufferData == NULL, false);


    s32 capacity = (1 << 18) * (sizeof(CarpToken) + 4);
    CARP_ASSERT_RETURN(carp_buffer_create(capacity, 16, outASTBuffer), false);




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
        sPrintErrorAt(&tokenIndex);
        CARP_LOGERROR("Failed to parse tokens\n");
        return false;
    }
    sPrintTokens(&tokenIndex.tokenIndexBuffer, &tokenOutBuffer);

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
