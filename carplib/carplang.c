#include "carplang.h"

#include "carpassert.h"
#include "carplib.h"
#include "carplog.h"


typedef struct CarpLangBuffers CarpLangBuffers;
typedef struct CarpVariable CarpVariable;

static const s32 CarpBufferTokenMaxAmount = 1024 * 1024;
static const s32 CarpBufferASTMaxAmount = 1024 * 1024;
static const s32 CarpBufferVarsMaxAmount = 1024 * 1024;
static const s32 CarpBufferVarMemMaxAmount = 1024 * 64;

//////////////////////
//
// Token
//
/////////////////////


typedef enum CarpTokenType
{
    CarpTokenTypeUnknownToken,

    // Special characters or combinations
    CarpTokenTypeSpecialCharactersBegin,
    CarpTokenTypeEndOfTokens = CarpTokenTypeSpecialCharactersBegin, // '\0'
    CarpTokenTypePercentSign, // %
    CarpTokenTypePlus, // +
    CarpTokenTypeMinus, // -
    CarpTokenTypeStar, // *
    CarpTokenTypeSlash, // /
    CarpTokenTypeBackSlash, // '\'
    CarpTokenTypeComma, // ,
    CarpTokenTypeDot, // .
    CarpTokenTypeF, // 'f'
    CarpTokenTypeCommentLine, // //
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

    CarpTokenTypeSpecialCharactersEnd,

    // String type
    CarpTokenTypeString,

    // Number type
    CarpTokenTypeInt,
    CarpTokenTypeFloat,

    // Keywords
    CarpTokenTypeKeyWordsBegin,
    CarpTokenTypeConst = CarpTokenTypeKeyWordsBegin,
    CarpTokenTypeWhile,
    CarpTokenTypeFunc,
    CarpTokenTypeLet,
    CarpTokenTypeVar,
    CarpTokenTypeKeyWordsEnd,

    // Identifier
    CarpTokenTypeIdentifier,

    CarpTokenTypeCount,
} CarpTokenType;


typedef struct CarpToken
{
    union
    {
        const char* carpToken;
        f64 carpTokenFloat;
        s64 carpTokenInt;
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
    CarpTokenHelperLine("//", CarpTokenTypeCommentLine), // /
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
    CarpTokenHelperLine("|specialchars end|", CarpTokenTypeSpecialCharactersEnd),

    CarpTokenHelperLine("const", CarpTokenTypeConst),
    CarpTokenHelperLine("while", CarpTokenTypeWhile),
    CarpTokenHelperLine("fn", CarpTokenTypeFunc),
    CarpTokenHelperLine("let", CarpTokenTypeLet),
    CarpTokenHelperLine("var", CarpTokenTypeVar),
    CarpTokenHelperLine("|keywords end|", CarpTokenTypeKeyWordsEnd),


};
static const s32 CarpTokenHelperCount
    = (s32)(sizeof(CarpTokenHelpers) / sizeof(CarpTokenHelper));


#undef CarpTokenHelperLine
/*
typedef struct CarpTokenIndex
{
    const CarpBuffer carpTokenIndexBuffer;
    s32 carpTokenIndexIndex;
} CarpTokenIndex;
*/











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

    CarpExprTypeAssign,

    CarpExprTypeVar,
    CarpExprTypeConst,
    CarpExprTypeFn,

    CarpExprTypeInt,
    CarpExprTypeFloat,
    CarpExprTypeString,

    CarpExprTypeDefineVariable,
    CarpExprTypeDefineConst,

    CarpExprTypeCount,

} CarpExprType;

typedef enum CarpExprPresedenceType
{
    CarpExprPresedenceTypeUnknown,

    CarpExprPresedenceTypeFnCall,

    CarpExprPresedenceTypeNumber,
    CarpExprPresedenceTypeVariable,


    CarpExprPresedenceTypeMulDiv,
    CarpExprPresedenceTypeAddSub,

    CarpExprPresedenceTypeAssign,

    CarpExprPresedenceTypeDefineVariable,
    CarpExprPresedenceTypeDefineConst,


    CarpExprPresedenceTypeLowest,

    CarpExprPresedenceTypeCount,
} CarpExprPresedenceType;

typedef enum CarpExprFlag
{
    CarpExprFlagConst = 0x0001,

} CarpExprFlag;

typedef struct CarpExpr
{
    union
    {
        struct
        {
            s32 carpExprLeft;
            s32 carpExprRight;
        };
        s64 carpExprInt;
        f64 carpExprFloat;
        f32 carpExprV4[4];
    };
    CarpExprType carpExprType;
    CarpExprPresedenceType carpExprPresedence;
    CarpExprFlag carpExprFlags;
    s32 carpExprPadding;
} CarpExpr;
static_assert(sizeof(CarpExpr) == 32, "CarpExpr expected size doesnt match");


typedef struct CarpTokenIndexer
{
    s32 carpTokenIndexerIndex;
} CarpTokenIndexer;
static_assert(sizeof(CarpTokenIndexer) == 4, "CarpTokenIndexer expected size doesnt match");

typedef s32 (*CarpPresedenceFn)(struct CarpTokenIndexer* tokenIndexer, s32 leftIndex,
    CarpLangBuffers* outBuffers);

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


struct CarpVariable
{
    s32 carpVariableTokenIndexName;
    s32 carpVariableMemoryAddressOffset;
    s32 carpVariableMemorySize;
    CarpVariableType carpVariableTypeFlags;
};
static_assert(sizeof(CarpVariable) == 16, "CarpVariable expected size doesnt match");




static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer, const CarpLangBuffers* buffers);
static bool sGetTokenIndex(s32 tokenIndex, const CarpLangBuffers* buffers, const CarpToken** outToken);
static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type, const CarpLangBuffers* buffers);
static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers);
static s32 sCheckIfVariableExists(const CarpToken* nameToken, const CarpLangBuffers* buffers);
static CarpVariable* sGetVariableFromTokenIndex(s32 nameTokenIndex, const CarpLangBuffers* buffers);
static u8* sEvalVarOffset(s32 varMemOffset, const CarpLangBuffers* buffers);

static s32 sParseExpression(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    CarpLangBuffers* outBuffers);

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outMem);
static bool sPrintExpressionHelper(s32 index,
    CarpLangBuffers* langMem,
    s64* outValue);


static s32 sParseMulDiv(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpLangBuffers* outBuffers);
static s32 sParseAddSub(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpLangBuffers* outBuffers);


CarpPresedence CarpPresedences[] =
{
    { &sParseMulDiv, CarpExprPresedenceTypeMulDiv },
    { &sParseAddSub, CarpExprPresedenceTypeAddSub },

};
static const s32 CarpPresedeceCount = (s32)(sizeof(CarpPresedences) / sizeof(CarpPresedences[0]));



//////////////
//
// Common
//
//////////////

struct CarpLangBuffers
{
    const char* carpLangBuffersInput;

    CarpToken* carpLangBuffersTokens;
    CarpExpr* carpLangBuffersAST;
    CarpBuffer carpLangBuffersStatements;
    CarpVariable* carpLangBuffersVars;
    u8* carpLangBuffersVarValues;

    CarpBuffer carpLangBuffersProgram;

    s32 carpLangBuffersInputCount;

    s32 carpLangBuffersTokenCount;
    s32 carpLangBuffersASTCount;

    s32 carpLangBuffersStatementCount;
    s32 carpLangBuffersVarsCount;
    s32 carpLangBuffersProgramCount;

    s32 carpLangBuffersCurrentMemoryBufferOffset;

};



//////////////
//
// Token
//
//////////////

static bool sSkipWhiteSpace(const char** pos)
{
    CARP_ASSERT_RETURN(pos, false);
    while(**pos)
    {
        switch(**pos)
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return true;
        }
        ++(*pos);
    }
    return true;
}


static s32 sTryParseIdentifier(const char** pos)
{
    CARP_ASSERT_RETURN(pos, -1);

    char c = **pos;
    if(!(
        (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || c == '_'))
    {
        return 0;
    }
    s32 len = 1;
    ++(*pos);
    while(**pos)
    {
        char c = **pos;
        if(!(
            (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || c == '_'))
        {
            return len;
        }
        ++(*pos);
        ++len;
    }
    return -1;
}

static bool sPushToken(const CarpToken* token, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(token, false);
    CARP_ASSERT_RETURN(outBuffers, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersTokenCount < CarpBufferTokenMaxAmount, false);

    outBuffers->carpLangBuffersTokens[outBuffers->carpLangBuffersTokenCount] = *token;
    outBuffers->carpLangBuffersTokenCount++;

    return true;
}

static bool sParseTokens(s32* inputIndex, CarpLangBuffers *outBuffers)
{
    CARP_ASSERT_RETURN(inputIndex, false);
    CARP_ASSERT_RETURN(outBuffers, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersInput, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersInputCount > 0, false);

    const char* pos = outBuffers->carpLangBuffersInput;

    while(*inputIndex < outBuffers->carpLangBuffersInputCount)
    {
        CarpToken pushToken = { 0 };

        CARP_ASSERT_RETURN(sSkipWhiteSpace(&pos), false);
        if(*pos == '\0')
        {
            pushToken.carpTokenType = CarpTokenTypeEndOfTokens;
            pushToken.carpTokenLen = 1;

            CARP_ASSERT_RETURN(sPushToken(&pushToken, outBuffers), false);
            return true;
        }
        pushToken.carpToken = pos;

        // Print start of token
        //CARP_LOG("W: %-.*s\n", 4, pos);

        const char* prev = pos;
        s32 idLen = sTryParseIdentifier(&pos);
        CARP_ASSERT_RETURN(idLen >= 0, false);


        if(idLen > 0)
        {
            pushToken.carpTokenType = CarpTokenTypeIdentifier;
            pushToken.carpTokenLen = idLen;

            s32 helperIndex = 0;
            while(CarpTokenHelpers[helperIndex].carpTokenHelperType != CarpTokenTypeKeyWordsBegin)
                ++helperIndex;

            while(CarpTokenHelpers[helperIndex].carpTokenHelperType != CarpTokenTypeKeyWordsEnd)
            {
                const CarpTokenHelper* helper = CarpTokenHelpers + helperIndex;
                if(idLen ==  helper->carpTokenHelperLen
                    && carp_lib_memcmp(helper->carpTokenHelperStr, prev, helper->carpTokenHelperLen) == 0)
                {
                    idLen = helper->carpTokenHelperLen;

                    pushToken.carpTokenType = helper->carpTokenHelperType;
                    pushToken.carpTokenLen = helper->carpTokenHelperLen;
                    break;
                }
                ++helperIndex;
            }
        }
        else if(carp_lib_isnumber(*pos))
        {
            const char* end = NULL;

            pushToken.carpTokenInt = carp_lib_strtoll(pos, &end, 10);
            s32 numLen = (s32)(end - pos);
            CARP_ASSERT_RETURN(numLen > 0, false);
            pushToken.carpTokenType = CarpTokenTypeInt;

            if(*end == '.')
            {
                pushToken.carpTokenFloat = carp_lib_strtod(pos, &end);
                pushToken.carpTokenType = CarpTokenTypeFloat;
            }
            pos = end;
            CARP_ASSERT_RETURN(pos, false);

            pushToken.carpTokenLen = numLen;

        }
        else
        {
            s32 helperIndex = 0;
            while(CarpTokenHelpers[helperIndex].carpTokenHelperType != CarpTokenTypeSpecialCharactersBegin)
                ++helperIndex;

            while(CarpTokenHelpers[helperIndex].carpTokenHelperType != CarpTokenTypeSpecialCharactersEnd)
            {
                const CarpTokenHelper* helper = CarpTokenHelpers + helperIndex;
                if(carp_lib_memcmp(helper->carpTokenHelperStr, pos, helper->carpTokenHelperLen) == 0)
                {
                    pushToken.carpTokenType = helper->carpTokenHelperType;
                    pushToken.carpTokenLen = helper->carpTokenHelperLen;

                    pos += helper->carpTokenHelperLen;

                    if(helper->carpTokenHelperType == CarpTokenTypeDoubleQuote)
                    {
                        bool isEscape = false;
                        while(*pos && ( *pos != '\"' || isEscape ))
                        {
                            isEscape = !isEscape && *pos == '\\';
                            ++pos;
                        }
                        CARP_ASSERT_RETURN(*pos == '\"', false);
                        ++pos;
                    }
                    else if(helper->carpTokenHelperType == CarpTokenTypeCommentLine)
                    {
                        while(*pos && *pos != '\n')
                        {
                            ++pos;
                        }
                        ++pos;
                    }
                    else if(helper->carpTokenHelperType == CarpTokenTypeCommentBlockBegin)
                    {
                        while(*pos && !(*pos == '*' && *(pos + 1) == '/'))
                        {
                            ++pos;
                        }
                        CARP_ASSERT_RETURN(*pos == '*' && *(pos + 1) == '/', false);
                        pos += 2;
                    }
                    break;
                }
                ++helperIndex;
            }
            if(pushToken.carpTokenType == CarpTokenTypeCommentLine
                || pushToken.carpTokenType == CarpTokenTypeCommentBlockBegin)
            {
                continue;
            }

            else if(pushToken.carpTokenType == CarpTokenTypeDoubleQuote)
            {
                pushToken.carpTokenType = CarpTokenTypeString;
                pushToken.carpTokenLen = pos - pushToken.carpToken;
            }
        }
        CARP_ASSERT_RETURN(pushToken.carpTokenType != CarpTokenTypeUnknownToken, false);
        CARP_ASSERT_RETURN(sPushToken(&pushToken, outBuffers), false);

        *inputIndex = (s32)(pos - outBuffers->carpLangBuffersInput);
        CARP_ASSERT_RETURN(*inputIndex < outBuffers->carpLangBuffersInputCount, false);
    }

    return false;
}

static void sPrintTokenParseErrorAt(s32 inputIndex, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, ;);
    CARP_ASSERT_RETURN(inputIndex <= buffers->carpLangBuffersInputCount, ;);

    s32 lineNumber = 0;
    s32 charNumber = 0;

    s32 index = 0;
    s32 nextLineIndex = 0;
    while(index < inputIndex)
    {
        if(buffers->carpLangBuffersInput[index] == '\n')
        {
            lineNumber++;
            charNumber = 0;
        }
        ++charNumber;
        ++index;
    }
    nextLineIndex = index + 1;
    while(nextLineIndex < buffers->carpLangBuffersTokenCount)
    {
        char c = buffers->carpLangBuffersInput[nextLineIndex];
        if(c == '\n' || c == '\0')
        {
            break;
        }
        ++nextLineIndex;
    }

    // print 8 chars left from error
    s32 printLenL = inputIndex > 8 ? 8 : inputIndex;
    s32 printLenR = nextLineIndex - inputIndex;
    s32 printLen = printLenL +  printLenR;
    index = inputIndex - printLenL;

    CARP_LOGERROR("Error at: %i:%i \"%-.*s\"\n",
        lineNumber,
        charNumber,
        printLen,
        buffers->carpLangBuffersInput + index);

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
        case CarpTokenTypeSpecialCharactersEnd: name = "|special characters end|"; break;

        case CarpTokenTypeCommentLine: name = "//"; break;
        case CarpTokenTypeCommentBlockBegin: name = "/*"; break;

        // String type
        case CarpTokenTypeString: name = ""; break;

        // Number type
        case CarpTokenTypeInt: name = "s64"; break;
        case CarpTokenTypeFloat: name = "f64"; break;

        // Keywords
        case CarpTokenTypeConst: name = "Const"; break;
        case CarpTokenTypeWhile: name = "While"; break;
        case CarpTokenTypeFunc: name = "Func"; break;
        case CarpTokenTypeLet: name = "Let"; break;
        case CarpTokenTypeVar: name = "Var"; break;
        case CarpTokenTypeKeyWordsEnd: name = "|keywords end|"; break;

        case CarpTokenTypeIdentifier: name = "Identifier"; break;
        case CarpTokenTypeCount: name = "Count"; break;
    }
    if(token->carpTokenType == CarpTokenTypeIdentifier
        || token->carpTokenType == CarpTokenTypeString)
    {
        CARP_LOG("'%s[%-.*s]', ", name, token->carpTokenLen, token->carpToken);
    }
    else if(token->carpTokenType == CarpTokenTypeInt)
    {
        CARP_LOG("'%s[%" PRIi64 "]'", name, token->carpTokenInt);
    }
    else if(token->carpTokenType == CarpTokenTypeFloat)
    {
        CARP_LOG("'%s[%f]'", name, token->carpTokenFloat);
    }
    else
    {
        CARP_LOG("'%s', ", name);
    }
}

static void sPrintTokens(const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, ;);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, ;);

    const CarpToken* token = (const CarpToken*)buffers->carpLangBuffersTokens;

    for(s32 tokenCount = 0; tokenCount < buffers->carpLangBuffersTokenCount; ++ tokenCount)
    {
        if((tokenCount % 8) == 0)
        {
            CARP_LOG("\n");
        }
        sPrintToken(token);
        ++token;
    }
    CARP_LOG("\n");
}


















//////////////////////
//
// AST
//
/////////////////////







static void sPrintTokenFromCarpTokenIndexer(
    const CarpTokenIndexer* tokenIndexer,
    const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, ;);
    CARP_ASSERT_RETURN(buffers, ;);

    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, buffers, &token), ;);

    CARP_LOG("Token: ");
    sPrintToken(token);
    CARP_LOG("index: %i\n", tokenIndexer->carpTokenIndexerIndex);
}

static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(buffers, false);

    CARP_ASSERT_RETURN(tokenIndexer->carpTokenIndexerIndex < buffers->carpLangBuffersTokenCount, false);
    ++tokenIndexer->carpTokenIndexerIndex;
    return true;
}

static bool sGetTokenIndex(s32 tokenIndex,
    const CarpLangBuffers* buffers,
    const CarpToken** outToken)
{
    CARP_ASSERT_RETURN(outToken, false);
    CARP_ASSERT_RETURN(buffers, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(tokenIndex <= buffers->carpLangBuffersTokenCount, false);

    *outToken = buffers->carpLangBuffersTokens + tokenIndex - 1;
    return true;
}


static s32 sPushVariable(const CarpVariable* var, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(var, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersVars, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersVarsCount < CarpBufferVarsMaxAmount, -1);

    s32 result = outBuffers->carpLangBuffersVarsCount;
    outBuffers->carpLangBuffersVarsCount++;
    outBuffers->carpLangBuffersVars[result] = *var;
    return result + 1;

}

static s32 sPushExpr(const CarpExpr* expr, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(expr, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersAST, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersASTCount < CarpBufferASTMaxAmount, -1);

    s32 result = outBuffers->carpLangBuffersASTCount;
    outBuffers->carpLangBuffersASTCount++;
    outBuffers->carpLangBuffersAST[result] = *expr;
    return result + 1;
}

static s32 sPushMemOffset(s32 variableMemorySize, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(
        outBuffers->carpLangBuffersCurrentMemoryBufferOffset + variableMemorySize <= CarpBufferVarMemMaxAmount, -1);
    s32 alignment = variableMemorySize < 16 ? variableMemorySize : 16;
    s32 alignMinusOne = alignment - 1;

    s32 result = outBuffers->carpLangBuffersCurrentMemoryBufferOffset;
    // make the offset....
    outBuffers->carpLangBuffersCurrentMemoryBufferOffset = (result + alignMinusOne) & (~alignMinusOne);

    return result + 1;
}

static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, buffers, &next), false);
    if(next->carpTokenType == type)
        sAdvanceToken(tokenIndexer, buffers);
    return next->carpTokenType == type;
}

static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersAST, -1);

    s32 currentIndex = tokenIndexer->carpTokenIndexerIndex;

    bool neg = false;
    while(true)
    {
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeMinus, outBuffers))
        {
            neg = !neg;
        }
        else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypePlus, outBuffers))
        {
        }
        else
        {
            break;
        }
    }
    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, outBuffers, &next), false);

    CarpExpr expr = {0};
    expr.carpExprPresedence = CarpExprPresedenceTypeNumber;
    expr.carpExprFlags = CarpExprFlagConst;
    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeInt, outBuffers))
    {
        expr.carpExprType = CarpExprTypeInt;
        expr.carpExprInt = neg ? -next->carpTokenInt : next->carpTokenInt;
        //CARP_LOGINFO("s64: %" PRIi64 "\n", expr.carpExprInt);
    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeFloat, outBuffers))
    {
        expr.carpExprType = CarpExprTypeFloat;
        expr.carpExprFloat = neg ? -next->carpTokenFloat : next->carpTokenFloat;
        //CARP_LOGINFO("f64: %f\n", expr.carpExprFloat);
    }
    else
    {
        tokenIndexer->carpTokenIndexerIndex = currentIndex;
        return 0;
    }
    s32 result = sPushExpr(&expr, outBuffers);
    CARP_ASSERT_RETURN(result > 0, -1);
    return result;
}


static s32 sParseNext(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersAST, -1);
    s32 exprValue = 0;
    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesOpen, outBuffers))
    {
        exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
        CARP_ASSERT_RETURN(exprValue >= 0, -1);
        CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesClose, outBuffers), -1);
    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIdentifier, outBuffers))
    {

        // fn call, multiple expressions for parameters.
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesOpen, outBuffers))
        {
            do
            {
                exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
                CARP_ASSERT_RETURN(exprValue >= 0, -1);
                // do something with parameters....
            } while(exprValue);

            CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeParenthesesClose, outBuffers), -1);
        }

        // identifier[expr] indexing array
        else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketOpen, outBuffers))
        {
            exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
            CARP_ASSERT_RETURN(exprValue >= 0, -1);

            // need index....

            CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketClose, outBuffers), -1);
        }
        else
        {
            s32 nameTokenIndex = tokenIndexer->carpTokenIndexerIndex - 1;
            const CarpVariable* var = sGetVariableFromTokenIndex(nameTokenIndex, outBuffers);
            CARP_ASSERT_RETURN(var, -1);

            CarpExpr newExpr = { 0 };

            // TODO: Make const use own buffer for values?
            if(var->carpVariableTypeFlags & CarpVariableTypeConst)
            {
            }
            CarpVariableType vType =
                CarpVariableTypeInt
                | CarpVariableTypeFloat;

            CARP_ASSERT_RETURN(var->carpVariableTypeFlags & vType, -1);

            newExpr.carpExprType = CarpExprTypeVar;
            newExpr.carpExprLeft = var->carpVariableMemoryAddressOffset;
            newExpr.carpExprPresedence = CarpExprPresedenceTypeVariable;
            if(var->carpVariableTypeFlags & CarpVariableTypeConst)
            {
                newExpr.carpExprFlags = CarpExprFlagConst;
                newExpr.carpExprType = CarpExprTypeConst;
            } 
            exprValue = sPushExpr(&newExpr, outBuffers);
            CARP_ASSERT_RETURN(exprValue > 0, -1);
        }
        // const/variable array

    }
    else
    {
        // number
        exprValue = sParseNumber(tokenIndexer, outBuffers);
        CARP_ASSERT_RETURN(exprValue >= 0, -1);

    }



    return exprValue;
}







static CarpVariable* sGetVariable(s32 index, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, NULL);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, NULL);

    CARP_ASSERT_RETURN(index > 0, NULL);
    CARP_ASSERT_RETURN(index - 1 < buffers->carpLangBuffersVarsCount, NULL);

    return (buffers->carpLangBuffersVars + index - 1);
}

static CarpVariable* sGetVariableFromMemOffset(s32 memOffset, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, NULL);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, NULL);

    CARP_ASSERT_RETURN(memOffset > 0, NULL);
    CARP_ASSERT_RETURN(memOffset - 1 < buffers->carpLangBuffersCurrentMemoryBufferOffset, NULL);

    for(s32 index = 0; index < buffers->carpLangBuffersVarsCount; ++index)
    {
        CarpVariable* var = &buffers->carpLangBuffersVars[index];
        if(var->carpVariableMemoryAddressOffset == memOffset)
        {
            return var;
        }
    }

    return NULL;
}

static bool sSetVariableValue(
    const CarpVariable* var,
    const u8* newValue, 
    CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(outBuffers, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersVars, false);

    CARP_ASSERT_RETURN(var, false);
    CARP_ASSERT_RETURN(newValue, false);

    s32 memOffset = var->carpVariableMemoryAddressOffset;

    CARP_ASSERT_RETURN(memOffset > 0, false);
    CARP_ASSERT_RETURN(memOffset - 1 < outBuffers->carpLangBuffersCurrentMemoryBufferOffset, false);
    CARP_ASSERT_RETURN(var->carpVariableMemorySize > 0, false);
    CARP_ASSERT_RETURN(var->carpVariableMemorySize <= 4, false);
    
    u8* value = outBuffers->carpLangBuffersVarValues + memOffset - 1;
    carp_lib_memcopy(value, newValue, var->carpVariableMemorySize);

    return true;
}



static CarpVariable* sGetVariableFromTokenIndex(s32 nameTokenIndex, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, NULL);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, NULL);
    CARP_ASSERT_RETURN(nameTokenIndex > 0, NULL);


    const CarpToken* nameToken;
    CARP_ASSERT_RETURN(sGetTokenIndex(nameTokenIndex, buffers, &nameToken), false);

    s32 variableIndex = sCheckIfVariableExists(nameToken, buffers);
    CARP_ASSERT_RETURN(variableIndex > 0, false);

    CarpVariable* var = sGetVariable(variableIndex, buffers);
    CARP_ASSERT_RETURN(var, false);
    return var;
}

static CarpExpr* sGetMutExpr(s32 index, CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, NULL);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersAST, NULL);

    CARP_ASSERT_RETURN(index > 0, NULL);
    CARP_ASSERT_RETURN(index - 1 < buffers->carpLangBuffersASTCount, NULL);

    return buffers->carpLangBuffersAST + (index - 1);
}

static const CarpExpr* sGetExpr(s32 index, const CarpLangBuffers* buffers)
{
    return sGetMutExpr(index, (CarpLangBuffers*)buffers);
}


static s32 sParseExpressionRight(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    s32 leftIndex,
    CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);
    for(s32 i = 0; i < CarpPresedeceCount; ++i)
    {
        if(CarpPresedences[i].carpPresedenceType < presedence)
        {
            s32 expr = CarpPresedences[i].carpPresedenceFn(tokenIndexer, leftIndex, outBuffers);
            CARP_ASSERT_RETURN(expr >= 0, -1);
            if(expr)
            {
                return expr;
            }
        }
    }
    return leftIndex;
}

typedef struct EvalHelper
{
    union
    {
        s64 evalHelperSValue;
        f64 evalHelperFValue;
    };
    CarpVariableType evalHelperType;
} EvalHelper;

static bool sEvalExprHelper(const CarpExpr* expr, const CarpLangBuffers* buffers, EvalHelper* outValue)
{
    CARP_ASSERT_RETURN(expr, false);
    CARP_ASSERT_RETURN(buffers, false);
    CARP_ASSERT_RETURN(outValue, false);

    //if((expr->carpExprFlags & CarpExprFlagConst) == 0)
    //    return 0;


    EvalHelper rValue = { 0 };

    const CarpVariable* var = NULL;
    const u8* varValue = NULL;
    const CarpExpr* lExpr = NULL;
    const CarpExpr* rExpr = NULL;

    switch(expr->carpExprType)
    {
        case CarpExprTypeInt:
            outValue->evalHelperSValue = expr->carpExprInt;
            outValue->evalHelperType = CarpVariableTypeInt;
            return true;

        case CarpExprTypeFloat:
            outValue->evalHelperFValue = expr->carpExprFloat;
            outValue->evalHelperType = CarpVariableTypeFloat;
            return true;

        case CarpExprTypeVar:
        case CarpExprTypeConst:
            var = sGetVariableFromMemOffset(expr->carpExprLeft, buffers);
            CARP_ASSERT_RETURN(var, false);
            varValue = (const u8*)sEvalVarOffset(var->carpVariableMemoryAddressOffset, buffers);
            CARP_ASSERT_RETURN(varValue, false);

            if(var->carpVariableTypeFlags & CarpVariableTypeFloat)
            {
                outValue->evalHelperType = CarpVariableTypeFloat;
                carp_lib_memcopy(&outValue->evalHelperFValue, varValue, 4);
            }
            if(var->carpVariableTypeFlags & CarpVariableTypeInt)
            {
                outValue->evalHelperType = CarpVariableTypeInt;
                carp_lib_memcopy(&outValue->evalHelperSValue, varValue, 4);
            }
            return true;

        case CarpExprTypeAdd:
        case CarpExprTypeSub:
        case CarpExprTypeDiv:
        case CarpExprTypeMul:
            lExpr = sGetExpr(expr->carpExprLeft, buffers);
            rExpr = sGetExpr(expr->carpExprRight, buffers);
            if(!sEvalExprHelper(lExpr, buffers, outValue))
                return false;
            if(!sEvalExprHelper(rExpr, buffers, &rValue))
                return false;
            break;

        default:
            CARP_ASSERT_RETURN(0 && "Not impelmented", false);
    }

    CARP_ASSERT_RETURN(outValue->evalHelperType == rValue.evalHelperType, false);

    switch(outValue->evalHelperType)
    {
        case CarpVariableTypeInt:
            switch(expr->carpExprType)
            {
                case CarpExprTypeAdd: outValue->evalHelperSValue += rValue.evalHelperSValue; break;
                case CarpExprTypeSub: outValue->evalHelperSValue -= rValue.evalHelperSValue; break;
                case CarpExprTypeDiv: outValue->evalHelperSValue /= rValue.evalHelperSValue; break;
                case CarpExprTypeMul: outValue->evalHelperSValue *= rValue.evalHelperSValue; break;

                default: break;
            }

            break;

        case CarpVariableTypeFloat:

            switch(expr->carpExprType)
            {
                case CarpExprTypeAdd: outValue->evalHelperFValue += rValue.evalHelperFValue; break;
                case CarpExprTypeSub: outValue->evalHelperFValue -= rValue.evalHelperFValue; break;
                case CarpExprTypeDiv: outValue->evalHelperFValue /= rValue.evalHelperFValue; break;
                case CarpExprTypeMul: outValue->evalHelperFValue *= rValue.evalHelperFValue; break;

                default: break;
            }

            break;

        default:
            CARP_ASSERT_RETURN(0 && "variable type unknown", false);
    }

    return true;
}

static u8* sEvalVarOffset(s32 varMemOffset, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers, NULL);
    CARP_ASSERT_RETURN(varMemOffset > 0, NULL);

    const CarpVariable* var = sGetVariableFromMemOffset(varMemOffset, buffers);
    CARP_ASSERT_RETURN(var, NULL);
    CARP_ASSERT_RETURN(varMemOffset + var->carpVariableMemorySize - 1 <= buffers->carpLangBuffersCurrentMemoryBufferOffset, NULL);

    return buffers->carpLangBuffersVarValues + varMemOffset - 1;
}

static s32 sEvalExpr(const CarpExpr* expr, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(expr, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);

    //if((expr->carpExprFlags & CarpExprFlagConst) == 0)
    //    return 0;

    EvalHelper value = { 0 };
    CARP_ASSERT_RETURN(sEvalExprHelper(expr, outBuffers, &value), -1);

    CarpExpr newExpr = { 0 };

    switch(value.evalHelperType)
    {
        case CarpVariableTypeInt:
            newExpr.carpExprType = CarpExprTypeInt;
            newExpr.carpExprInt = value.evalHelperSValue;
            break;
        case CarpVariableTypeFloat:
            newExpr.carpExprType = CarpExprTypeFloat;
            newExpr.carpExprFloat = value.evalHelperFValue;
            break;
        default:
            break;
    }
    newExpr.carpExprFlags = CarpExprFlagConst;
    newExpr.carpExprPresedence = CarpExprPresedenceTypeNumber;

    s32 result = sPushExpr(&newExpr, outBuffers);
    return result;
}

static s32 sParseExpression(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersAST, -1);

    s32 expr = 0;

    s32 leftIndex = sParseNext(tokenIndexer, outBuffers);
    CARP_ASSERT_RETURN(leftIndex >= 0, -1);
    if(leftIndex == 0)
    {
        return 0;
    }

    while((expr = sParseExpressionRight(tokenIndexer, presedence, leftIndex, outBuffers))
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
    CarpLangBuffers* langMem,
    s64* outValue
)
{
    s64 tmpL = 0;
    s64 tmpR = 0;
    CARP_ASSERT_RETURN(oper, false);
    CARP_LOG("(");
    CARP_ASSERT_RETURN(sPrintExpressionHelper(leftIndex, langMem, &tmpL), false);
    CARP_LOG(" %s ", oper);
    CARP_ASSERT_RETURN(sPrintExpressionHelper(rightIndex, langMem, &tmpR), false);
    CARP_LOG(")");

    if(*oper == '+') *outValue = tmpL + tmpR;
    else if(*oper == '-') *outValue = tmpL - tmpR;
    else if(*oper == '*') *outValue = tmpL * tmpR;
    else if(*oper == '/') *outValue = tmpL / tmpR;
    else { CARP_LOG("unknown symbol\n"); }

    //CARP_LOG("%"PRIi64 " %s %" PRIi64 " = %" PRIi64 "\n", tmpL, oper, tmpR, *outValue );

    return true;
}

static bool sPrintNameFromVariable(const CarpVariable* var, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, false);
    CARP_ASSERT_RETURN(var, false);

    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(var->carpVariableTokenIndexName, buffers, &token), false);

    CARP_LOG("%.*s", token->carpTokenLen, token->carpToken);


    return true;
}

static bool sPrintVarIdentifier(s32 index, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(index > 0, false);
    CARP_ASSERT_RETURN(buffers, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, false);
    CARP_ASSERT_RETURN(index - 1 < buffers->carpLangBuffersCurrentMemoryBufferOffset, false);

    const CarpVariable* var = sGetVariableFromMemOffset(index, buffers);
    return sPrintNameFromVariable(var, buffers);
}

static bool sPrintExpressionHelper(
    s32 index,
    CarpLangBuffers* langMem,
    s64* outValue)
{
    #define TMP_SWITCH_HELPER(helperType, helperOper) \
        case helperType: \
            CARP_ASSERT_RETURN( \
                sPrintExprLeftRight( \
                    expr->carpExprLeft, \
                    expr->carpExprRight, \
                    helperOper, \
                    langMem, \
                    outValue), \
                false); \
            break

    const CarpExpr* expr = sGetExpr(index, langMem);
    const CarpVariable* var = NULL;
    CARP_ASSERT_RETURN(expr, false);
    switch(expr->carpExprType)
    {
        TMP_SWITCH_HELPER(CarpExprTypeAdd, "+");
        TMP_SWITCH_HELPER(CarpExprTypeSub, "-");
        TMP_SWITCH_HELPER(CarpExprTypeMul, "*");
        TMP_SWITCH_HELPER(CarpExprTypeDiv, "/");

        case CarpExprTypeInt:
            CARP_LOG("%" PRIi64, expr->carpExprInt);
            *outValue = expr->carpExprInt;
            break;
        case CarpExprTypeFloat:
            CARP_LOG("%f", expr->carpExprFloat);
            break;
        case CarpExprTypeVar:
        case CarpExprTypeConst:
            var = sGetVariableFromMemOffset(expr->carpExprLeft, langMem);
            const u8* varValue = sEvalVarOffset(var->carpVariableMemoryAddressOffset, langMem);
            CARP_ASSERT_RETURN(varValue, false);
            if(var->carpVariableTypeFlags & CarpVariableTypeFloat)
                CARP_LOG("%f",  *((const float*)varValue));
            else if(var->carpVariableTypeFlags & CarpVariableTypeInt)
                CARP_LOG("%i", *((const s32*)varValue));
            carp_lib_memcopy(outValue, varValue, var->carpVariableMemorySize);
            break;

        case CarpExprTypeAssign:
            var = sGetVariableFromMemOffset(expr->carpExprLeft, langMem);
            CARP_ASSERT_RETURN(sPrintNameFromVariable(var, langMem), false);
            CARP_LOG(" := ");
            CARP_ASSERT_RETURN(sPrintExpressionHelper(expr->carpExprRight, langMem, outValue), false);
            if(var->carpVariableTypeFlags & CarpVariableTypeFloat)
                CARP_LOG(" = %f",  *((const float*)outValue));
            else if(var->carpVariableTypeFlags & CarpVariableTypeInt)
                CARP_LOG(" = %i", *((const s32*)outValue));
            
            sSetVariableValue(var, (const u8*)outValue, langMem);
            break;

        case CarpExprTypeDefineConst:
            CARP_LOG("const ");
            var = sGetVariableFromMemOffset(expr->carpExprLeft, langMem);
            CARP_ASSERT_RETURN(sPrintNameFromVariable(var, langMem), false);
            CARP_LOG(" := ");
            CARP_ASSERT_RETURN(sPrintExpressionHelper(
                    expr->carpExprRight,
                    langMem,
                    outValue)
                , false);
            sSetVariableValue(var, (const u8*)outValue, langMem);

            break;
        case CarpExprTypeDefineVariable:
            CARP_LOG("var ");
            var = sGetVariableFromMemOffset(expr->carpExprLeft, langMem);
            CARP_ASSERT_RETURN(sPrintNameFromVariable(var, langMem), false);
            CARP_LOG(" := ");
            CARP_ASSERT_RETURN(sPrintExpressionHelper(
                    expr->carpExprRight,
                    langMem,
                    outValue)
                , false);
            sSetVariableValue(var, (const u8*)outValue, langMem);
            break;

        default:
            break;

    }


    #undef TMP_SWITCH_HELPER
    return true;
}
static bool sPrintExpression(s32 index, CarpLangBuffers* langMem)
{
    CARP_LOG("Expr: ");
    s64 tmpValue = 0;

    const CarpExpr* expr = sGetExpr(index, langMem);


    CARP_ASSERT_RETURN(sPrintExpressionHelper(index, langMem, &tmpValue), false);
    CARP_LOG(" = %" PRIi64 ", is const: %i\n",
        tmpValue,
        expr ? (expr->carpExprFlags & CarpExprFlagConst) : 0);
    return true;
}

static s32 sCheckIfVariableExists(const CarpToken* nameToken, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(nameToken, -1);
    CARP_ASSERT_RETURN(buffers, -1);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, -1);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, -1);

    for(s32 i = 0; i < buffers->carpLangBuffersVarsCount; ++i)
    {
        const CarpVariable* var = buffers->carpLangBuffersVars + i;
        const CarpToken* otherToken = NULL;
        CARP_ASSERT_RETURN(sGetTokenIndex(var->carpVariableTokenIndexName, buffers, &otherToken), -1);

        if(otherToken->carpTokenLen == nameToken->carpTokenLen && carp_lib_memcmp(
            otherToken->carpToken, nameToken->carpToken, nameToken->carpTokenLen) == 0)
        {
            return i + 1;
        }
    }
    return 0;
}


static s32 sParseVariable(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);

    s32 nameTokenIndex = tokenIndexer->carpTokenIndexerIndex;
    const CarpToken* nameToken;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, outBuffers, &nameToken), false);
    CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIdentifier, outBuffers), -1);

    CARP_ASSERT_RETURN(sCheckIfVariableExists(nameToken, outBuffers) == 0, -1);

    CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeAssign, outBuffers), -1);

    s32 exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
    CARP_ASSERT_RETURN(exprValue > 0, -1);

    const CarpExpr* rightExpr = sGetExpr(exprValue, outBuffers);
    CARP_ASSERT_RETURN(rightExpr > 0, -1);


    s64 variableMemorySize = 0;
    CarpVariable variable = { 0 };
    variable.carpVariableTokenIndexName = nameTokenIndex;
    if(rightExpr->carpExprType == CarpExprTypeFloat)
    {
        variable.carpVariableTypeFlags = CarpVariableTypeFloat;
        variableMemorySize = 4;
    }
    else if(rightExpr->carpExprType == CarpExprTypeInt)
    {
        variable.carpVariableTypeFlags = CarpVariableTypeInt;
        variableMemorySize = 4;
    }
    else if(rightExpr->carpExprType == CarpExprTypeVar)
    {
        const CarpVariable* constVar = sGetVariableFromMemOffset(
            rightExpr->carpExprLeft, outBuffers);
        CARP_ASSERT_RETURN(constVar, -1);
        
        variable.carpVariableTypeFlags = constVar->carpVariableTypeFlags;
        variable.carpVariableTypeFlags &= (~CarpVariableTypeConst);
        
        variableMemorySize = constVar->carpVariableMemorySize;
    }

    else //if(rightExpr->carpExprFlags & CarpExprFlagConst)
    {
        exprValue = sEvalExpr(rightExpr, outBuffers);
        CARP_ASSERT_RETURN(exprValue > 0, -1);
        const CarpExpr* evalExpr = sGetExpr(exprValue, outBuffers);
        CARP_ASSERT_RETURN(evalExpr > 0, -1);

        if(evalExpr->carpExprFlags & CarpVariableTypeInt)
            variable.carpVariableTypeFlags = CarpVariableTypeInt;
        else if(evalExpr->carpExprFlags & CarpVariableTypeFloat)
            variable.carpVariableTypeFlags = CarpVariableTypeFloat;
        else
        {
            CARP_ASSERT_RETURN(0 && "Unknown type", -1);
        }
        variableMemorySize = 4;
    }

    CARP_ASSERT_RETURN(variableMemorySize > 0, -1);
    variable.carpVariableMemorySize = variableMemorySize;
    variable.carpVariableMemoryAddressOffset = sPushMemOffset(variableMemorySize, outBuffers);
    CARP_ASSERT_RETURN(variable.carpVariableMemoryAddressOffset > 0, -1);

    outBuffers->carpLangBuffersCurrentMemoryBufferOffset += variableMemorySize;

    s32 defineVar = sPushVariable(&variable, outBuffers);
    CARP_ASSERT_RETURN(defineVar > 0, -1);

    CarpExpr expr = { 0 };
    expr.carpExprLeft = variable.carpVariableMemoryAddressOffset;
    expr.carpExprRight = exprValue;
    expr.carpExprType = CarpExprTypeDefineVariable;
    expr.carpExprPresedence = CarpExprPresedenceTypeDefineVariable;

    s32 defineExpr = sPushExpr(&expr, outBuffers);
    CARP_ASSERT_RETURN(defineExpr > 0, -1);

    return defineExpr;
}

static s32 sParseConst(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);

    s32 nameTokenIndex = tokenIndexer->carpTokenIndexerIndex;
    s32 result = sParseVariable(tokenIndexer, outBuffers);

    CARP_ASSERT_RETURN(result > 0, -1);
    CarpExpr* expr = sGetMutExpr(result, outBuffers);
    expr->carpExprType = CarpExprTypeDefineConst;
    expr->carpExprPresedence = CarpExprPresedenceTypeDefineConst;

    const CarpExpr* rightExpr = sGetExpr(expr->carpExprRight, outBuffers);
    if(rightExpr->carpExprFlags & CarpExprFlagConst)
        expr->carpExprFlags |= CarpExprFlagConst;

    CarpVariable* var = sGetVariableFromTokenIndex(nameTokenIndex, outBuffers);
    CARP_ASSERT_RETURN(var, -1);

    var->carpVariableTypeFlags |= CarpVariableTypeConst;

    return result;
}

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, false);
    CARP_ASSERT_RETURN(outBuffers, false);

    const CarpToken* next = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, outBuffers, &next), false);

    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeConst, outBuffers))
    {
        s32 expr = sParseConst(tokenIndexer, outBuffers);
        CARP_ASSERT_RETURN(expr >= 0, false);
        sPrintExpression(expr, outBuffers);
        return true;
    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeVar, outBuffers))
    {
        s32 expr = sParseVariable(tokenIndexer, outBuffers);
        CARP_ASSERT_RETURN(expr >= 0, false);
        sPrintExpression(expr, outBuffers);
        return true;
    }

    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIdentifier, outBuffers))
    {
        s32 nameTokenIndex = tokenIndexer->carpTokenIndexerIndex - 1;
        const CarpVariable* var = sGetVariableFromTokenIndex(nameTokenIndex, outBuffers);
        CARP_ASSERT_RETURN(var, false);

        CARP_ASSERT_RETURN((var->carpVariableTypeFlags & CarpVariableTypeConst) == 0, false);


        // No other implementations than assignment after var
        CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeAssign, outBuffers), false);
        s32 exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
        CARP_ASSERT_RETURN(exprValue >= 0, false);
        CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSemiColon, outBuffers), false);


        const CarpExpr* rExpr = sGetExpr(exprValue, outBuffers);
        CARP_ASSERT_RETURN(rExpr, false);

        CarpExpr newExpr = { 0 };
        newExpr.carpExprType = CarpExprTypeAssign;
        if(rExpr->carpExprType == CarpExprTypeInt)
        {
            CARP_ASSERT_RETURN(
                (var->carpVariableTypeFlags &  CarpVariableTypeInt), false);

        }
        if(rExpr->carpExprType == CarpExprTypeFloat)
        {
            CARP_ASSERT_RETURN(
                (var->carpVariableTypeFlags &  CarpVariableTypeFloat), false);

        }
        newExpr.carpExprPresedence = CarpExprPresedenceTypeAssign;
        newExpr.carpExprRight = exprValue;
        newExpr.carpExprLeft = var->carpVariableMemoryAddressOffset;

        s32 expr = sPushExpr(&newExpr, outBuffers);
        CARP_ASSERT_RETURN(expr > 0, false);

        if(expr > 0)
        {
            sPrintExpression(expr, outBuffers);
        }

        return true;
    }


    s32 expr = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
    CARP_ASSERT_RETURN(expr >= 0, false);
    CARP_ASSERT_RETURN(expr == 0 || sAdvanceIfToken(tokenIndexer, CarpTokenTypeSemiColon, outBuffers), false);
    if(expr > 0)
    {
        sPrintExpression(expr, outBuffers);
    }
    return expr;
}

static bool sParseAST(CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(outBuffers, false);

    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersAST, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersVars, false);
    CARP_ASSERT_RETURN(outBuffers->carpLangBuffersTokens, false);

    CarpTokenIndexer indexer = {0};

    while(indexer.carpTokenIndexerIndex < outBuffers->carpLangBuffersTokenCount)
    {
        if(!sParseStatement(&indexer, outBuffers))
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
    CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);

    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(tokenIndexer->carpTokenIndexerIndex, outBuffers, &token), -1);

    s32 currentIndex = tokenIndexer->carpTokenIndexerIndex;

    if(sAdvanceIfToken(tokenIndexer, type, outBuffers))
    {
        CarpExpr expr = {0};
        expr.carpExprType = exprType;
        expr.carpExprPresedence = presedenceType;

        s32 rightIndex = sParseExpression(tokenIndexer, presedenceType, outBuffers);
        expr.carpExprLeft = leftIndex;
        expr.carpExprRight = rightIndex;
        const CarpExpr* left = sGetExpr(leftIndex, outBuffers);
        const CarpExpr* right = sGetExpr(rightIndex, outBuffers);

        bool isConstExpr = 
            left 
            && (left->carpExprFlags & CarpExprFlagConst) != 0 
            && (!right || (right->carpExprFlags & CarpExprFlagConst) != 0);
        expr.carpExprFlags = isConstExpr ? CarpExprFlagConst : 0;
        s32 result = sPushExpr(&expr, outBuffers);
        CARP_ASSERT_RETURN(result > 0, -1);
        return result;
    }
    return 0;
}

static s32 sParseMulDiv(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);

    s32 expr = sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeStar, CarpExprTypeMul, CarpExprPresedenceTypeMulDiv, outBuffers);
    expr |= sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeSlash, CarpExprTypeDiv, CarpExprPresedenceTypeMulDiv, outBuffers);
    CARP_ASSERT_RETURN(expr >= 0, -1);
    return expr;
}


static s32 sParseAddSub(CarpTokenIndexer* tokenIndexer, s32 leftIndex, CarpLangBuffers* outBuffers)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outBuffers, -1);
    CARP_ASSERT_RETURN(leftIndex > 0, -1);

    s32 expr = sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypePlus, CarpExprTypeAdd, CarpExprPresedenceTypeAddSub, outBuffers);
    expr |= sParseBinomRight(
        tokenIndexer, leftIndex, CarpTokenTypeMinus, CarpExprTypeSub, CarpExprPresedenceTypeAddSub, outBuffers);
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
    CarpRegTypeR08,
    CarpRegTypeR09,
    CarpRegTypeR10,
    CarpRegTypeR11,
    CarpRegTypeR12,
    CarpRegTypeR13,
    CarpRegTypeR14,
    CarpRegTypeR15,

    CarpRegTypeRIP,
    CarpRegTypeRFlags,

    CarpRegTypeXMM00,
    CarpRegTypeXMM01,
    CarpRegTypeXMM02,
    CarpRegTypeXMM03,
    CarpRegTypeXMM04,
    CarpRegTypeXMM05,
    CarpRegTypeXMM06,
    CarpRegTypeXMM07,
    CarpRegTypeXMM08,
    CarpRegTypeXMM09,
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



    CarpLangBuffers mem = {
        .carpLangBuffersInput = input,
        .carpLangBuffersInputCount = inputLen,
    };
    mem.carpLangBuffersTokens = carp_lib_calloc(sizeof(CarpToken), CarpBufferTokenMaxAmount);
    mem.carpLangBuffersAST = carp_lib_calloc(sizeof(CarpExpr), CarpBufferASTMaxAmount);
    mem.carpLangBuffersVars = carp_lib_calloc(sizeof(CarpVariable), CarpBufferVarsMaxAmount);
    mem.carpLangBuffersVarValues = carp_lib_calloc(sizeof(u8), CarpBufferVarMemMaxAmount);


    CARP_ASSERT_RETURN(mem.carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(mem.carpLangBuffersAST, false);
    CARP_ASSERT_RETURN(mem.carpLangBuffersVars, false);


    s32 tokenIndex = 0;
    if(!sParseTokens(&tokenIndex, &mem))
    {
        sPrintTokenParseErrorAt(tokenIndex, &mem);
        CARP_LOGERROR("Failed to parse tokens\n");
        return false;
    }
    sPrintTokens(&mem);

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

    carp_lib_free(mem.carpLangBuffersVarValues);
    carp_lib_free(mem.carpLangBuffersVars);
    carp_lib_free(mem.carpLangBuffersTokens);
    carp_lib_free(mem.carpLangBuffersAST);
    return true;
}


