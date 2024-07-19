#include "carplang.h"

#include "carpassert.h"
#include "carplib.h"
#include "carplog.h"


typedef struct CarpLangBuffers CarpLangBuffers;

static const s32 CarpBufferTokenMaxAmount = 1024 * 1024;
static const s32 CarpBufferASTMaxAmount = 1024 * 1024;
static const s32 CarpBufferVarsMaxAmount = 1024 * 1024;

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
    CarpTokenTypeIntNumber,
    CarpTokenTypeRealNumber,

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


    CarpExprTypeVarName,
    CarpExprTypeConstName,
    CarpExprTypeFnName,
    CarpExprTypeIntNumber,
    CarpExprTypeRealNumber,
    CarpExprTypeString,

    CarpExprTypeDefineVariable,
    CarpExprTypeDefineConst,

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
        s64 carpExprIntNumber;
        f64 carpExprRealNumber;
        f32 carpExprV4[4];
    };
    CarpExprType carpExprType;
    s32 carpExprStart;
    CarpExprPresedenceType carpExprPresedence;
    CarpExprFlag carpExprFlags;
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


typedef struct CarpVariable
{
    s32 carpVariableTokenIndexName;
    s32 carpVariableMemoryAddressOffset;
    CarpVariableType carpVariableTypeFlags;
    s32 carpVariableMemoryPadding;
} CarpVariable;
static_assert(sizeof(CarpVariable) == 16, "CarpVariable expected size doesnt match");




static bool sAdvanceToken(CarpTokenIndexer* tokenIndexer, const CarpLangBuffers* buffers);
static bool sGetTokenIndex(s32 tokenIndex, const CarpLangBuffers* buffers, const CarpToken** outToken);
static bool sAdvanceIfToken(CarpTokenIndexer* tokenIndexer, CarpTokenType type, const CarpLangBuffers* buffers);
static s32 sParseNumber(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outBuffers);

static s32 sParseExpression(
    CarpTokenIndexer* tokenIndexer,
    CarpExprPresedenceType presedence,
    CarpLangBuffers* outBuffers);

static bool sParseStatement(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outMem);
static bool sPrintExpressionHelper(s32 index,
    const CarpLangBuffers* langMem,
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

    CarpBuffer carpLangBuffersProgram;

    s32 carpLangBuffersInputCount;

    s32 carpLangBuffersTokenCount;
    s32 carpLangBuffersASTCount;

    s32 carpLangBuffersStatementCount;
    s32 carpLangBuffersVarsCount;
    s32 carpLangBuffersProgramCount;

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

            pushToken.carpTokenIntNumber = carp_lib_strtoll(pos, &end, 10);
            s32 numLen = (s32)(end - pos);
            CARP_ASSERT_RETURN(numLen > 0, false);
            pushToken.carpTokenType = CarpTokenTypeIntNumber;

            if(*end == '.')
            {
                pushToken.carpTokenRealNumber = carp_lib_strtod(pos, &end);
                pushToken.carpTokenType = CarpTokenTypeRealNumber;
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
        case CarpTokenTypeIntNumber: name = "s64"; break;
        case CarpTokenTypeRealNumber: name = "f64"; break;

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
    expr.carpExprStart = currentIndex;
    expr.carpExprPresedence = CarpExprPresedenceTypeNumberVariable;
    expr.carpExprFlags = CarpExprFlagConst;
    if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeIntNumber, outBuffers))
    {
        expr.carpExprType = CarpExprTypeIntNumber;
        expr.carpExprIntNumber = neg ? -next->carpTokenIntNumber : next->carpTokenIntNumber;
        //CARP_LOGINFO("s64: %" PRIi64 "\n", expr.carpExprIntNumber);
    }
    else if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeRealNumber, outBuffers))
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
        if(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketOpen, outBuffers))
        {
            exprValue = sParseExpression(tokenIndexer, CarpExprPresedenceTypeLowest, outBuffers);
            CARP_ASSERT_RETURN(exprValue >= 0, -1);

            // need index....

            CARP_ASSERT_RETURN(sAdvanceIfToken(tokenIndexer, CarpTokenTypeSquareBracketClose, outBuffers), -1);
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


static CarpExpr* sGetMutExpr(s32 index, CarpLangBuffers* buffer)
{
    CARP_ASSERT_RETURN(buffer, NULL);
    CARP_ASSERT_RETURN(buffer->carpLangBuffersAST, NULL);

    CARP_ASSERT_RETURN(index > 0, NULL);
    CARP_ASSERT_RETURN(index - 1 < buffer->carpLangBuffersASTCount, NULL);

    return buffer->carpLangBuffersAST + (index - 1);
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
    const CarpLangBuffers* langMem,
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

static bool sPrintIdentifier(s32 index, const CarpLangBuffers* buffers)
{
    CARP_ASSERT_RETURN(index > 0, false);
    CARP_ASSERT_RETURN(buffers, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersTokens, false);
    CARP_ASSERT_RETURN(buffers->carpLangBuffersVars, false);
    CARP_ASSERT_RETURN(index <= buffers->carpLangBuffersVarsCount, false);

    const CarpVariable* var = sGetVariable(index, buffers);
    const CarpToken* token = NULL;
    CARP_ASSERT_RETURN(sGetTokenIndex(var->carpVariableTokenIndexName, buffers, &token), false);

    CARP_LOG("%.*s", token->carpTokenLen, token->carpToken);


    return true;
}

static bool sPrintExpressionHelper(
    s32 index,
    const CarpLangBuffers* langMem,
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
        case CarpExprTypeDefineConst:
            CARP_LOG("const ");
            CARP_ASSERT_RETURN(sPrintIdentifier(expr->carpExprLeft, langMem),
                false);
            CARP_ASSERT_RETURN(sPrintExpressionHelper(
                    expr->carpExprRight,
                    langMem,
                    outValue)
                , false);
            break;
        case CarpExprTypeDefineVariable:
            CARP_LOG("var ");
            CARP_ASSERT_RETURN(sPrintIdentifier(expr->carpExprLeft, langMem),
                false);
            CARP_ASSERT_RETURN(sPrintExpressionHelper(
                    expr->carpExprRight,
                    langMem,
                    outValue)
                , false);
            break;

        default:
            break;

    }


    #undef TMP_SWITCH_HELPER
    return true;
}
static bool sPrintExpression(s32 index, const CarpLangBuffers* langMem)
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


    CarpVariable variable = { 0 };
    variable.carpVariableTokenIndexName = nameTokenIndex;
    if(rightExpr->carpExprType == CarpExprTypeRealNumber)
        variable.carpVariableTypeFlags = CarpVariableTypeFloat;
    else if(rightExpr->carpExprType == CarpExprTypeIntNumber)
        variable.carpVariableTypeFlags = CarpVariableTypeInt;

    s32 defineVar = sPushVariable(&variable, outBuffers);
    CARP_ASSERT_RETURN(defineVar > 0, -1);

    CarpExpr expr = { 0 };
    expr.carpExprStart = outBuffers->carpLangBuffersASTCount;
    expr.carpExprLeft = defineVar;
    expr.carpExprRight = exprValue;
    expr.carpExprType = CarpExprTypeDefineVariable;
    expr.carpExprPresedence = CarpExprPresedenceTypeDefineVariable;

    s32 defineExpr = sPushExpr(&expr, outBuffers);
    CARP_ASSERT_RETURN(defineExpr > 0, -1);

    return defineExpr;
}

static s32 sParseConst(CarpTokenIndexer* tokenIndexer, CarpLangBuffers* outMem)
{
    CARP_ASSERT_RETURN(tokenIndexer, -1);
    CARP_ASSERT_RETURN(outMem, -1);
    s32 result = sParseVariable(tokenIndexer, outMem);

    CARP_ASSERT_RETURN(result > 0, -1);
    CarpExpr* expr = sGetMutExpr(result, outMem);
    expr->carpExprType = CarpExprTypeDefineConst;
    expr->carpExprPresedence = CarpExprPresedenceTypeDefineConst;

    const CarpExpr* rightExpr = sGetExpr(expr->carpExprRight, outMem);
    if(rightExpr->carpExprFlags & CarpExprFlagConst)
        expr->carpExprFlags |= CarpExprFlagConst;
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
        if(sParseStatement(&indexer, outBuffers) == 0)
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
        expr.carpExprStart = currentIndex;
        expr.carpExprType = exprType;
        expr.carpExprPresedence = presedenceType;

        s32 rightIndex = sParseExpression(tokenIndexer, presedenceType, outBuffers);
        expr.carpExprLeft = leftIndex;
        expr.carpExprRight = rightIndex;
        const CarpExpr* left = sGetExpr(leftIndex, outBuffers);
        const CarpExpr* right = sGetExpr(rightIndex, outBuffers);

        bool isConstExpr = left != NULL;
        if(!(left && (left->carpExprFlags & CarpExprFlagConst)))
        {
            isConstExpr = false;
        }
        else if(right && !(left->carpExprFlags & CarpExprFlagConst))
        {
            isConstExpr = false;
        }
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

    carp_lib_free(mem.carpLangBuffersVars);
    carp_lib_free(mem.carpLangBuffersTokens);
    carp_lib_free(mem.carpLangBuffersAST);
    return true;
}


