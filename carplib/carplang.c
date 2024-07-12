#include "carplang.h"

#include "carpassert.h"
#include "carplib.h"
#include "carplog.h"

typedef struct CarpLangMemoryHelpers
{
    s32 caarpLangMemoryHelpersVariableMemorySize; // current size of variable memory
} CarpLangMemoryHelpers;

typedef struct CarpLangMemory
{
    CarpBuffer carpLangMemoryTokens;
    CarpBuffer carpLangMemoryAST;
    CarpBuffer carpLangMemoryStatements;
    CarpBuffer carpLangMemoryVars;
    CarpBuffer carpLangMemoryConsts;

    CarpBuffer carpLangMemoryProgram;


    CarpLangMemoryHelpers carpLangMemoryHelpers;

} CarpLangMemory;


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

    CarpTokenTypeAssign, // :=
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
    CarpTokenTypeIntNumber,
    CarpTokenTypeRealNumber,

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
    union
    {
        const u8* carpToken;
        f64 carpTokenRealNumber;
        s64 carpTokenIntNumber;
    };
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
    CarpTokenHelperLine(":=", CarpTokenTypeAssign), // :=
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
    const CarpBuffer carpTokenIndexBuffer;
    s32 carpTokenIndexIndex;
} CarpTokenIndex;

static bool sSkipWhiteSpace(CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexIndex < tokenIndex->carpTokenIndexBuffer.carp_buffer_size, false);
    CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexBuffer.carp_buffer_data, false);

    while(tokenIndex->carpTokenIndexIndex < tokenIndex->carpTokenIndexBuffer.carp_buffer_size)
    {
        switch(tokenIndex->carpTokenIndexBuffer.carp_buffer_data[tokenIndex->carpTokenIndexIndex])
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return true;
        }
        ++tokenIndex->carpTokenIndexIndex;
    }
    return false;
}


static s32 sTryParseIdentifier(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, -1);
    CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexIndex < tokenIndex->carpTokenIndexBuffer.carp_buffer_size, -1);
    CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexBuffer.carp_buffer_data, -1);

    char c = tokenIndex->carpTokenIndexBuffer.carp_buffer_data[tokenIndex->carpTokenIndexIndex];
    if(!(
        (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || c == '_'))
    {
        return 0;
    }
    s32 len = 1;

    while(tokenIndex->carpTokenIndexIndex + len < tokenIndex->carpTokenIndexBuffer.carp_buffer_size)
    {
        char c = tokenIndex->carpTokenIndexBuffer.carp_buffer_data[tokenIndex->carpTokenIndexIndex + len];
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

static bool sPushToken(const CarpToken* token, CarpBuffer *outTokenBuffer)
{
    return carp_buffer_pushBuffer(outTokenBuffer, (const u8*)token, sizeof(CarpToken));
}

static bool sParseTokens(CarpTokenIndex* tokenIndex, CarpBuffer *outTokenBuffer)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(outTokenBuffer, false);

    const CarpBuffer* input = &(tokenIndex->carpTokenIndexBuffer);
    CARP_ASSERT_RETURN(input->carp_buffer_size > 0, false);
    CARP_ASSERT_RETURN(input->carp_buffer_data, false);
    CARP_ASSERT_RETURN(input->carp_buffer_data[input->carp_buffer_size - 1] == '\0', false);

    CARP_ASSERT_RETURN(carp_buffer_create((1 << 20), 16, outTokenBuffer), false);

    const u8* pos = input->carp_buffer_data;

    while(tokenIndex->carpTokenIndexIndex < input->carp_buffer_size)
    {
        CarpToken pushToken = { 0 };
        tokenIndex->carpTokenIndexIndex = pos - input->carp_buffer_data;

        CARP_ASSERT_RETURN(sSkipWhiteSpace(tokenIndex), false);
        CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexIndex < input->carp_buffer_size, false);
        pos = input->carp_buffer_data + tokenIndex->carpTokenIndexIndex;
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
                && carp_lib_memcmp(helper->carpTokenHelperStr, pos, helper->carpTokenHelperLen) == 0)
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
                    while(tokenIndex->carpTokenIndexIndex < input->carp_buffer_size
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
                    while(tokenIndex->carpTokenIndexIndex < input->carp_buffer_size
                        && *pos != '\0' && *pos != '\n')
                    {
                        ++pos;
                    }
                }
                else if(helper->carpTokenHelperType == CarpTokenTypeCommentBlockBegin)
                {
                    while(tokenIndex->carpTokenIndexIndex < input->carp_buffer_size
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
            const char* end = NULL;
            const char* p = (const char*)pos;

            pushToken.carpTokenIntNumber = carp_lib_strtoll(p, &end, 10);
            s32 numLen = (s32)(end - p);
            CARP_ASSERT_RETURN(numLen > 0, false);
            pushToken.carpTokenType = CarpTokenTypeIntNumber;

            if(*end == '.')
            {
                pushToken.carpTokenRealNumber = carp_lib_strtod(p, &end);
                pushToken.carpTokenType = CarpTokenTypeRealNumber;
            }
            pos += end - p;

            pushToken.carpTokenLen = numLen;
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
static void sPrintTokenErrorAt(const CarpTokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, ;);
    CARP_ASSERT_RETURN(tokenIndex->carpTokenIndexBuffer.carp_buffer_data, ;);

    s32 lineNumber = 0;
    s32 charNumber = 0;

    s32 index = 0;
    s32 nextLineIndex = 0;
    while(index < tokenIndex->carpTokenIndexIndex)
    {
        char c = (char)tokenIndex->carpTokenIndexBuffer.carp_buffer_data[index];
        if(c == '\n')
        {
            lineNumber++;
            charNumber = 0;
        }
        ++charNumber;
        ++index;
    }
    nextLineIndex = index + 1;
    while(nextLineIndex < tokenIndex->carpTokenIndexBuffer.carp_buffer_size)
    {
        char c = (char)tokenIndex->carpTokenIndexBuffer.carp_buffer_data[nextLineIndex];
        if(c == '\n' || c == '\0')
        {
            break;
        }
        ++nextLineIndex;
    }


    s32 printLenL = (tokenIndex->carpTokenIndexIndex) > 5 ? 5 : tokenIndex->carpTokenIndexIndex;
    s32 printLenR = nextLineIndex - tokenIndex->carpTokenIndexIndex;
    s32 printLen = printLenL +  printLenR;
    index = tokenIndex->carpTokenIndexIndex - printLenL;

    CARP_LOGERROR("Error at: %i:%i \"%-.*s\"\n",
        lineNumber,
        charNumber,
        printLen,
        tokenIndex->carpTokenIndexBuffer.carp_buffer_data + index);

}

static void sPrintToken(const CarpToken* token)
{
    CARP_ASSERT_RETURN(token, ;);

    const char* name = {0};
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
        case CarpTokenTypeAssign: name = ":="; break;
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
        case CarpTokenTypeIntNumber: name = "s64"; break;
        case CarpTokenTypeRealNumber: name = "f64"; break;

        // Keywords
        case CarpTokenTypeConst: name = "Const"; break;
        case CarpTokenTypeWhile: name = "While"; break;
        case CarpTokenTypeFunc: name = "Func"; break;
        case CarpTokenTypeLet: name = "Let"; break;
        case CarpTokenTypeVar: name = "Var"; break;

        case CarpTokenTypeIdentifier: name = "Identifier"; break;
        case CarpTokenTypeCount: name = "Count"; break;
    }
    if(token->carpTokenType == CarpTokenTypeIdentifier
        || token->carpTokenType == CarpTokenTypeString)
    {
        CARP_LOG("'%s[%-.*s]', ", name, token->carpTokenLen, token->carpToken);
    }
    else if(token->carpTokenType == CarpTokenTypeIntNumber)
    {
        CARP_LOG("'%s[%" PRIi64 "]'", name, token->carpTokenIntNumber);
    }
    else if(token->carpTokenType == CarpTokenTypeRealNumber)
    {
        CARP_LOG("'%s[%f]'", name, token->carpTokenRealNumber);
    }
    else
    {
        CARP_LOG("'%s', ", name);
    }
}

static void sPrintTokens(const CarpBuffer* input, const CarpBuffer* tokenBuffer)
{
    CARP_ASSERT_RETURN(input, ;);
    CARP_ASSERT_RETURN(tokenBuffer, ;);

    const CarpToken* token = (const CarpToken*)tokenBuffer->carp_buffer_data;
    const CarpToken* endToken = (const CarpToken*)
        (tokenBuffer->carp_buffer_data + tokenBuffer->carp_buffer_size);
    s32 tokenCount = 0;
    while(token < endToken)
    {
        if((tokenCount % 8) == 0)
        {
            CARP_LOG("\n");
        }
        sPrintToken(token);
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
    CarpExprTypeUnknown,

    CarpExprTypeAdd,
    CarpExprTypeSub,
    CarpExprTypeMul,
    CarpExprTypeDiv,


    CarpExprTypeVarName,
    CarpExprTypeConstName,
    CarpExprTypeFnName,
    CarpExprTypeIntNumber,
    CarpExprTypeRealNumber,
    CarpExprString,

    CarpExprTypeCount,

} CarpExprType;

typedef enum CarpExprPresedenceType
{
    CarpExprPresedenceTypeUnknown,

    CarpExprPresedenceTypeFnCall,

    CarpExprPresedenceTypeMulDiv,
    CarpExprPresedenceTypeAddSub,
    CarpExprPresedenceTypeNumberVariable,

    CarpExprPresedenceTypeDefineVariable,
    CarpExprPresedenceTypeDefineConst,


    CarpExprPresedenceTypeLowest,

    CarpExprPresedenceTypeCount,
} CarpExprPresedenceType;

typedef struct CarpExpr
{
    union
    {
        struct
        {
            s32 carpExprLeft;
            s32 carpExprRight;
        };
        s64 carpExprIntNumber;
        f64 carpExprRealNumber;
        f32 carpExprV4[4];
    };
    CarpExprType carpExprType;
    s32 carpExprStart;
    CarpExprPresedenceType carpExprPresedence;
    s32 carpExprPadding;
} CarpExpr;
static_assert(sizeof(CarpExpr) == 32, "CarpExpr expected size doesnt match");


typedef struct CarpTokenIndexer
{
    const CarpToken* carpTokenIndexerTokens;
    s32 carpTokenIndexerTokenAmount;
    s32 carpTokenIndexerIndex;
} CarpTokenIndexer;
static_assert(sizeof(CarpTokenIndexer) == 16, "CarpTokenIndexer expected size doesnt match");

typedef s32 (*CarpPresedenceFn)(struct CarpTokenIndexer* tokenIndexer, s32 leftIndex,
    CarpBuffer* outASTBuffer);

typedef struct CarpPresedence
{
    CarpPresedenceFn carpPresedenceFn;
    CarpExprPresedenceType carpPresedenceType;
    s32 carpPresedencePadding;
} CarpPresedence;
static_assert(sizeof(CarpPresedence) == 16, "CarpPresedence expected size doesnt match");


typedef enum CarpVariableType
{
    CarpVariableTypeInt        = 0x01,
    CarpVariableTypeFloat      = 0x02,
    CarpVariableTypeV4         = 0x04,
    CarpVariableTypeString     = 0x08,
    CarpVariableTypeConst      = 0x10,
} CarpVariableType;


typedef struct CarpVariable
{
    s32 carpVariableTokenIndexName;
    s32 carpVariableMemoryAddress;
    CarpVariableType carpVariableTypeFlags;
    s32 carpVariableMemoryPadding;
} CarpVariable;
static_assert(sizeof(CarpVariable) == 16, "CarpVariable expected size doesnt match");







static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer);
static bool sGetToken(const CarpTokenIndexer* tokenIndexer, const CarpToken** outToken);
static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type);
static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer);

static s32 sParseExpression(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    CarpBuffer* outASTBuffer);

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpLangMemory* outMem);
static bool sPrintExpressionHelper(s32 index, const CarpBuffer* astBuffer, s64* outValue);


static s32 sParseMulDiv(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpBuffer* outASTBuffer);
static s32 sParseAddSub(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpBuffer* outASTBuffer);

CarpPresedence CarpPresedences[] =
{
    { &sParseMulDiv, CarpExprPresedenceTypeMulDiv },
    { &sParseAddSub, CarpExprPresedenceTypeAddSub },

};
static const s32 CarpPresedeceCount = (s32)(sizeof(CarpPresedences) / sizeof(CarpPresedences[0]));



static void sPrintTokenFromCarpTokenIndexer(const CarpTokenIndexer* tokenIndexer)
{
    CARP_ASSERT_RETURN(tokenIndexer, ;);

    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetToken(tokenIndexer, &token), ;);

    CARP_LOG("Token: ");
    sPrintToken(token);
    CARP_LOG("index: %i\n", tokenIndexer->carpTokenIndexerIndex);
}

static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(tokenIndexer->carpTokenIndexerIndex < tokenIndexer->carpTokenIndexerTokenAmount, false);
    ++tokenIndexer->carpTokenIndexerIndex;
    return true;
}

static bool sGetToken(const CarpTokenIndexer* tokenIndexer, const CarpToken** outToken)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outToken, false);
    CARP_ASSERT_RETURN(tokenIndexer->carpTokenIndexerIndex < tokenIndexer->carpTokenIndexerTokenAmount, false);

    *outToken = tokenIndexer->carpTokenIndexerTokens + tokenIndexer->carpTokenIndexerIndex;
    return true;
}

static s32 sPushExpr(const CarpExpr* expr, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(expr, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);

    s32 result = outASTBuffer->carp_buffer_size;
    carp_buffer_pushBuffer(outASTBuffer, (const u8*)expr, sizeof(CarpExpr));
    return result + 1;
}

static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetToken(tokenIndexer, &next), false);
    if(next->carpTokenType == type)
        sAdvanceToken(tokenIndexer);
    return next->carpTokenType == type;
}

static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);

    s32 currentIndex = tokenIndexer->carpTokenIndexerIndex;

    bool neg = false;
    while(true)
    {
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeMinus))
        {
            neg = !neg;
        }
        else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypePlus))
        {
        }
        else
        {
            break;
        }
    }
    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetToken(tokenIndexer, &next), false);

    CarpExpr expr = {0};
    expr.carpExprStart = currentIndex;
    expr.carpExprPresedence = CarpExprPresedenceTypeNumberVariable;

    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIntNumber))
    {
        expr.carpExprType = CarpExprTypeIntNumber;
        expr.carpExprIntNumber = neg ? -next->carpTokenIntNumber : next->carpTokenIntNumber;
        //CARP_LOGINFO("s64: %" PRIi64 "\n", expr.carpExprIntNumber);
    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeRealNumber))
    {
        expr.carpExprType = CarpExprTypeRealNumber;
        expr.carpExprRealNumber = neg ? -next->carpTokenRealNumber : next->carpTokenRealNumber;
        //CARP_LOGINFO("f64: %f\n", expr.carpExprRealNumber);
    }
    else
    {
        tokenIndexer->carpTokenIndexerIndex = currentIndex;
        return 0;
    }
    s32 result = sPushExpr(&expr, outASTBuffer);
    CARP_ASSERT_RETURN(result > 0, -1);
    return result;
}


static s32 sParseNext(CarpTokenIndexer* tokenIndexer, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);

    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesOpen))
    {
        s32 expr = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outASTBuffer);
        CARP_ASSERT_RETURN(expr >= 0, -1);
        CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesClose), -1);
        return expr;
    }
    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIdentifier))
    {
        // fn call, multiple expressions for parameters.
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesOpen))
        {
            s32 expr = 0;
            do
            {
                expr = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outASTBuffer);
                CARP_ASSERT_RETURN(expr >= 0, -1);
                // do something with parameters....
            } while(expr);

            CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesClose), -1);
        }

        // identifier[expr] indexing array
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketOpen))
        {
            s32 expr = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outASTBuffer);
            CARP_ASSERT_RETURN(expr >= 0, -1);

            // need index....

            CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketClose), -1);
        }
        // const/variable array

    }



    // number
    s32 numberResult = sParseNumber(tokenIndexer, outASTBuffer);
    CARP_ASSERT_RETURN(numberResult >= 0, -1);

    return numberResult;
}








static CarpExpr* sGetExpr(s32 index, const CarpBuffer* astBuffer)
{
    CARP_ASSERT_RETURN(astBuffer, NULL);

    CARP_ASSERT_RETURN(index > 0, NULL);
    CARP_ASSERT_RETURN(index - 1 + sizeof(CarpExpr) <= astBuffer->carp_buffer_size, NULL);

    return (CarpExpr*)(astBuffer->carp_buffer_data + (index - 1));
}



static s32 sParseExpressionRight(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    s32 leftIndex,
    CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);
    for(s32 i = 0; i < CarpPresedeceCount; ++i)
    {
        if(CarpPresedences[i].carpPresedenceType < presedence)
        {
            s32 expr = CarpPresedences[i].carpPresedenceFn(tokenIndexer, leftIndex, outASTBuffer);
            CARP_ASSERT_RETURN(expr >= 0, -1);
            if(expr)
            {
                return expr;
            }
        }
    }
    return leftIndex;
}

static s32 sParseExpression(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);

    s32 expr = 0;

    s32 leftIndex = sParseNext(tokenIndexer, outASTBuffer);
    CARP_ASSERT_RETURN(leftIndex >= 0, -1);
    if(leftIndex == 0)
    {
        return 0;
    }

    while((expr = sParseExpressionRight(tokenIndexer, presedence, leftIndex, outASTBuffer))
        && expr != leftIndex)
    {
        CARP_ASSERT_RETURN(expr >= 0, -1);
        leftIndex = expr;
    }

    return leftIndex;
}

static bool sPrintExprLeftRight(
    s32 leftIndex,
    s32 rightIndex,
    const char* oper,
    const CarpBuffer* astBuffer,
    s64* outValue
)
{
    s64 tmpL = 0;
    s64 tmpR = 0;
    CARP_ASSERT_RETURN(oper, false);
    CARP_LOG("(");
    CARP_ASSERT_RETURN(sPrintExpressionHelper(leftIndex, astBuffer, &tmpL), false);
    CARP_LOG(" %s ", oper);
    CARP_ASSERT_RETURN(sPrintExpressionHelper(rightIndex, astBuffer, &tmpR), false);
    CARP_LOG(")");

    if(*oper == '+') *outValue = tmpL + tmpR;
    else if(*oper == '-') *outValue = tmpL - tmpR;
    else if(*oper == '*') *outValue = tmpL * tmpR;
    else if(*oper == '/') *outValue = tmpL / tmpR;
    else { CARP_LOG("unknown symbol\n"); }

    //CARP_LOG("%"PRIi64 " %s %" PRIi64 " = %" PRIi64 "\n", tmpL, oper, tmpR, *outValue );

    return true;
}

static bool sPrintExpressionHelper(s32 index, const CarpBuffer* astBuffer, s64* outValue)
{
    #define TMP_SWITCH_HELPER(helperType, helperOper) \
        case helperType: \
            CARP_ASSERT_RETURN(sPrintExprLeftRight( \
                expr->carpExprLeft, \
                expr->carpExprRight, \
                helperOper, \
                astBuffer, outValue), false); \
            break

    const CarpExpr* expr = sGetExpr(index, astBuffer);
    CARP_ASSERT_RETURN(expr, false);
    switch(expr->carpExprType)
    {
        TMP_SWITCH_HELPER(CarpExprTypeAdd, "+");
        TMP_SWITCH_HELPER(CarpExprTypeSub, "-");
        TMP_SWITCH_HELPER(CarpExprTypeMul, "*");
        TMP_SWITCH_HELPER(CarpExprTypeDiv, "/");

        case CarpExprTypeIntNumber:
            CARP_LOG("%" PRIi64, expr->carpExprIntNumber);
            *outValue = expr->carpExprIntNumber;
            break;
        case CarpExprTypeRealNumber:
            CARP_LOG("%f", expr->carpExprRealNumber);
            break;
        default:
            break;

    }


    #undef TMP_SWITCH_HELPER
    return true;
}
static bool sPrintExpression(s32 index, const CarpBuffer* astBuffer)
{
    CARP_LOG("Expr: ");
    s64 tmpValue = 0;
    CARP_ASSERT_RETURN(sPrintExpressionHelper(index, astBuffer, &tmpValue), false);
    CARP_LOG(" = %" PRIi64 "\n", tmpValue);
    return true;
}

static s32 sCheckIfVariableExists(const CarpToken* token, const CarpBuffer* buffer)
{
    CARP_ASSERT_RETURN(token, -1);
    CARP_ASSERT_RETURN(buffer, -1);

    for(s32 i = 0; i < buffer->carp_buffer_size;)
    {
        if(carp_lib_memcmp(
            buffer->carp_buffer_data + i, token->carpToken, token->carpTokenLen) == 0)
        {
            return i + 1;
        }
        i += sizeof(CarpVariable);
    }
    return 0;
}


static s32 sParseVariable(CarpTokenIndexer* tokenIndexer, CarpLangMemory* outMem)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outMem, -1);

    const CarpToken* nameToken;
    sGetToken(tokenIndexer, &nameToken);
    CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIdentifier), -1);

    CARP_ASSERT_RETURN(sCheckIfVariableExists(nameToken, &outMem->carpLangMemoryConsts), -1);
    CARP_ASSERT_RETURN(sCheckIfVariableExists(nameToken, &outMem->carpLangMemoryVars), -1);

    CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeAssign), -1);

    s32 exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, &outMem->carpLangMemoryAST);
    CARP_ASSERT_RETURN(exprValue > 0, -1);

    CarpExpr expr = { 0 };
    expr.carpExprStart = outMem->carpLangMemoryAST.carp_buffer_size;

    return 1;
}

static s32 sParseConst(CarpTokenIndexer* tokenIndexer, CarpLangMemory* outMem)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outMem, -1);
    s32 result = sParseVariable(tokenIndexer, outMem);

    CARP_ASSERT_RETURN(result > 0, -1);

    return result;
}

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpLangMemory* outMem)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outMem, false);

    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeConst))
    {

    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeVar))
    {

    }


    s32 expr = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, &outMem->carpLangMemoryAST);
    CARP_ASSERT_RETURN(expr >= 0, false);
    CARP_ASSERT_RETURN(expr == 0 || sAdvanceIfToken(tokenIndexer, CarpTokenTypeSemiColon), false);
    if(expr > 0)
    {
        sPrintExpression(expr, &outMem->carpLangMemoryAST);


    }
    return expr;
}

static bool sParseAST(CarpLangMemory* outMem)
{
    CARP_ASSERT_RETURN(outMem, false);

    CARP_ASSERT_RETURN(carp_buffer_create((1 << 20), 16, &outMem->carpLangMemoryAST), false);


    const CarpToken* tokens = (const CarpToken*)&outMem->carpLangMemoryTokens.carp_buffer_data;
    CarpTokenIndexer indexer = {0};

    indexer.carpTokenIndexerTokens = tokens;
    indexer.carpTokenIndexerTokenAmount = outMem->carpLangMemoryTokens.carp_buffer_size / sizeof(CarpToken);

    while(indexer.carpTokenIndexerIndex < indexer.carpTokenIndexerTokenAmount)
    {
        if(sParseStatement(&indexer, outMem) == 0)
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


static s32 sParseBinomRight(
    CarpTokenIndexer* tokenIndexer,
    s32 leftIndex,
    CarpTokenType type,
    CarpExprType exprType,
    CarpExprPresedenceType presedenceType,
    CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);

    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetToken(tokenIndexer, &token), -1);

    s32 currentIndex = tokenIndexer->carpTokenIndexerIndex;

    if(sAdvanceIfToken(tokenIndexer, type))
    {
        CarpExpr expr = {0};
        expr.carpExprStart = currentIndex;
        expr.carpExprType = exprType;
        expr.carpExprPresedence = presedenceType;

        s32 rightIndex = sParseExpression(tokenIndexer, presedenceType, outASTBuffer);
        expr.carpExprLeft = leftIndex;
        expr.carpExprRight = rightIndex;


        s32 result = sPushExpr(&expr, outASTBuffer);
        CARP_ASSERT_RETURN(result > 0, -1);
        return result;
    }
    return 0;
}

static s32 sParseMulDiv(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);

    s32 expr = sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeStar, CarpExprTypeMul, CarpExprPresedenceTypeMulDiv, outASTBuffer);
    expr |= sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeSlash, CarpExprTypeDiv, CarpExprPresedenceTypeMulDiv, outASTBuffer);
    CARP_ASSERT_RETURN(expr >= 0, -1);
    return expr;
}


static s32 sParseAddSub(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpBuffer* outASTBuffer)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outASTBuffer, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);

    s32 expr = sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypePlus, CarpExprTypeAdd, CarpExprPresedenceTypeAddSub, outASTBuffer);
    expr |= sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeMinus, CarpExprTypeSub, CarpExprPresedenceTypeAddSub, outASTBuffer);
    CARP_ASSERT_RETURN(expr >= 0, -1);
    return expr;

}











///////////////////////////////////
//
// Codegen
//
///////////////////////////////////

// win
// volatile: RAX, RCX, RDX, R8, R9, R10, R11, and XMM0-XMM5
// linux...
// volatile RAX, RCX, RDX, RDI, RSI, R8, R9, R10, R11, not sure XMMs probably all volatile?

typedef enum CarpRegType
{
    CarpRegTypeRAX,
    CarpRegTypeRCX,
    CarpRegTypeRDX,
    CarpRegTypeRSI,
    CarpRegTypeRDI,
    CarpRegTypeRBP,
    CarpRegTypeRSP,
    CarpRegTypeRBX,
    CarpRegTypeR8,
    CarpRegTypeR9,
    CarpRegTypeR10,
    CarpRegTypeR11,
    CarpRegTypeR12,
    CarpRegTypeR13,
    CarpRegTypeR14,
    CarpRegTypeR15,

    CarpRegTypeRIP,
    CarpRegTypeRFlags,

    CarpRegTypeXMM0,
    CarpRegTypeXMM1,
    CarpRegTypeXMM2,
    CarpRegTypeXMM3,
    CarpRegTypeXMM4,
    CarpRegTypeXMM5,
    CarpRegTypeXMM6,
    CarpRegTypeXMM7,
    CarpRegTypeXMM8,
    CarpRegTypeXMM9,
    CarpRegTypeXMM10,
    CarpRegTypeXMM11,
    CarpRegTypeXMM12,
    CarpRegTypeXMM13,
    CarpRegTypeXMM14,
    CarpRegTypeXMM15,

} CarpRegType;








CARP_FN bool carp_lang_compileToSpecialBuffer(
    const char* input,
    s32 inputLen,
    CarpSpecialBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(input, false);
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(inputLen > 0, false);

    CarpLangMemory mem = {0};

    const CarpBuffer inputBuffer = {
            .carp_buffer_data = (u8*)input,
            .carp_buffer_capacity = inputLen,
            .carp_buffer_size = inputLen,
            .carp_buffer_alignment = 1,
    };

    CarpTokenIndex tokenIndex = {.carpTokenIndexBuffer = inputBuffer };
    if(!sParseTokens(&tokenIndex, &mem.carpLangMemoryTokens))
    {
        sPrintTokenErrorAt(&tokenIndex);
        CARP_LOGERROR("Failed to parse tokens\n");
        return false;
    }
    sPrintTokens(&tokenIndex.carpTokenIndexBuffer, &mem.carpLangMemoryTokens);

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

    if(!sParseAST(&mem))
    {
        CARP_LOGERROR("Failed to parse commands\n");
        return false;
    }

    carp_buffer_free(&mem.carpLangMemoryTokens);
    carp_buffer_free(&mem.carpLangMemoryAST);
    return true;
}
