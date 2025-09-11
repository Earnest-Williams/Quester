/* Copyright (C) 2001-2025 Val Argunov (byte AT qsp DOT org) */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "regexp.h"
#include "errors.h"
#include "text.h"

int qspCompiledRegExpsCurInd = 0;
QSPRegExp qspCompiledRegExps[QSP_MAXCACHEDREGEXPS];

void qspClearAllRegExps(QSP_BOOL toInit)
{
    int i;
    QSPRegExp *exp = qspCompiledRegExps;
    for (i = 0; i < QSP_MAXCACHEDREGEXPS; ++i)
    {
        if (!toInit && exp->CompiledExp)
        {
            qspFreeString(&exp->Text);
            onig_free(exp->CompiledExp);
        }
        exp->Text = qspNullString;
        exp->CompiledExp = 0;
        ++exp;
    }
    qspCompiledRegExpsCurInd = 0;
}

QSPRegExp *qspRegExpGetCompiled(QSPString exp)
{
    int i;
    regex_t *onigExp;
    OnigErrorInfo onigInfo;
    QSPRegExp *compExp = qspCompiledRegExps;
    for (i = 0; i < QSP_MAXCACHEDREGEXPS; ++i)
    {
        if (!compExp->CompiledExp) break;
        if (qspStrsEqual(exp, compExp->Text))
            return compExp;
        ++compExp;
    }
    if (onig_new(&onigExp, (OnigUChar *)exp.Str, (OnigUChar *)exp.End,
        ONIG_OPTION_DEFAULT, QSP_ONIG_ENC, ONIG_SYNTAX_PERL_NG, &onigInfo))
    {
        qspSetError(QSP_ERR_INCORRECTREGEXP);
        return 0;
    }
    compExp = qspCompiledRegExps + qspCompiledRegExpsCurInd;
    if (compExp->CompiledExp)
    {
        qspFreeString(&compExp->Text);
        onig_free(compExp->CompiledExp);
    }
    compExp->Text = qspCopyToNewText(exp);
    compExp->CompiledExp = onigExp;
    qspCompiledRegExpsCurInd = (qspCompiledRegExpsCurInd + 1) % QSP_MAXCACHEDREGEXPS;
    return compExp;
}

QSP_BOOL qspRegExpStrMatch(QSPRegExp *exp, QSPString str)
{
    OnigUChar *onigBeg = (OnigUChar *)str.Str, *onigEnd = (OnigUChar *)str.End;
    return (QSP_BOOL)(onig_match(exp->CompiledExp, onigBeg, onigEnd, onigBeg, 0, ONIG_OPTION_NONE) == onigEnd - onigBeg);
}

QSP_CHAR *qspRegExpStrSearch(QSPRegExp *exp, QSPString str, QSP_CHAR *startPos, int groupInd, int *foundLen)
{
    QSP_CHAR *foundPos = 0;
    OnigUChar *onigStart, *onigBeg = (OnigUChar *)str.Str, *onigEnd = (OnigUChar *)str.End;
    OnigRegion *onigReg = onig_region_new();
    onigStart = (startPos ? (OnigUChar *)startPos : onigBeg); /* allows correct searching within the whole string */
    if (onig_search(exp->CompiledExp, onigBeg, onigEnd, onigStart, onigEnd, onigReg, ONIG_OPTION_NONE) >= 0)
    {
        int pos = (groupInd > 0 ? groupInd : 0);
        if (pos < onigReg->num_regs && onigReg->beg[pos] >= 0)
        {
            foundPos = (QSP_CHAR *)(onigBeg + onigReg->beg[pos]);
            if (foundLen) *foundLen = (int)((onigReg->end[pos] - onigReg->beg[pos]) / sizeof(QSP_CHAR));
        }
    }
    if (foundLen && !foundPos) *foundLen = 0;
    onig_region_free(onigReg, 1);
    return foundPos;
}
