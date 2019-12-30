"""
Tool for internationalization

When you want to build dictionaries under languages folder, please run the command:
python i18ntool.py build

It will copy *.ts from source directories to languages folder, and generate
merged dictionaries (iRIC_XX_XX.ts) from separated dictionaries for each DLLs.

When you got iRIC_XX_XX.ts from translators, please run the two commands below:
python i18ntool.py split
python i18ntool.py apply

These command will split the iRIC_XX_XX.ts to split files, and then copy them
to source directories. Please note that the command just copies, so when the
source code has new words, they will not be included. In such cases, you should
run lupdate to add new words, or merge manually by yourself.

When you got dictionaries for DLLs (not merged version) fromtranslators, please run the one command below:
python i18ntool.py apply

"""
from pathlib import Path
import os
import re
import shutil
import sys

import iricdevtool as idev

MERGED_NAME = 'iRIC_'
LANG_FOLDER = '../languages'

def buildInitialLines(lang):
    lines = list()
    lines.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
    lines.append("<!DOCTYPE TS>\n")
    lines.append("<TS version=\"2.1\" language=\"" + lang + "\">\n")
    return lines

def getFileList():
    p = Path(LANG_FOLDER)
    return p.glob('*.ts')

def getLangList():
    s = set()
    prog = re.compile(r"_(.+_.+)\.ts$")
    for filename in getFileList():
        result = prog.search(str(filename))
        if not result:
            continue
        l = result.groups()[0]
        s.add(l)
    return list(s)

def mergeLangFiles(lang):
    print('Generating ' + MERGED_NAME + lang + '.ts...')
    lines = buildInitialLines(lang)

    for filename in getFileList():
        fname_str = str(filename)
        if not lang in fname_str:
            continue
        if MERGED_NAME in fname_str:
            continue

        f = open(filename, 'r', encoding='utf-8')
        contentMode = False
        lines.append("<!-- " + fname_str.replace('..\\languages\\', '') + " -->\n")
        for l in f.readlines():
            if '<TS' in l:
                contentMode = True
            elif '</TS>' in l:
                contentMode = False
            elif contentMode:
                lines.append(l)
        f.close()

    lines.append("</TS>\n")

    f = open(LANG_FOLDER + '/' + MERGED_NAME + lang + '.ts', 'w', encoding='utf-8')
    f.write("".join(lines))
    f.close()

def merge():
    for lang in getLangList():
        mergeLangFiles(lang)

def splitLangFile(lang):
    f = open(LANG_FOLDER + '/' + MERGED_NAME + lang + '.ts', 'r', encoding='utf-8')

    contentMode = False
    fileName = None
    lines = buildInitialLines(lang)
    for l in f.readlines():
        if not contentMode:
            if not "<!--" in l:
                continue
            result = re.search(r"<!\-\- (.+) ", l)
            fileName = result.groups()[0]

            contentMode = True
        else:
            result = re.search(r"<!\-\- (.+) ", l)
            if result or '</TS>' in l:
                lines.append("</TS>\n")
                f2 = open(LANG_FOLDER + '/' + fileName, 'w', encoding='utf-8')
                f2.write("".join(lines))
                f2.close()

            if result:
                fileName = result.groups()[0]
                lines = buildInitialLines(lang)
            else:
                lines.append(l)
    f.close()

def split():
    for lang in getLangList():
        splitLangFile(lang)

def copydics(path):
    if "debug" in path:
        return
    if "..\\languages" in path:
        return
    if "solvers" in path:
        return
    
    shutil.copy(path, LANG_FOLDER)
    print('Copied ' + path)

def copyFromSrc():
    idev.recursiveExec("..", "\.ts$", copydics)

def buildlist(path):
    global dictList

    if "debug" in path:
        return
    if "..\\languages" in path:
        return
    if "solvers" in path:
        return
    dictList.append(path)

def copyToSrc():
    global dictList
    dictList = list()

    idev.recursiveExec("..", "\.ts$", buildlist)

    for dictFile in dictList:
        p = Path(dictFile)
        name = LANG_FOLDER + '/' + p.name
        if os.path.exists(name):
            shutil.copy(name, dictFile)

def print_usage():
    print('Usage:')
    print('  python i18ntool.py build: Build dictionaries in ../languages from source')
    print('  python i18ntool.py split: Split dictionaries in ../languages')
    print('  python i18ntool.py apply: Copy dictionaries in ../languages to source')

if len(sys.argv) < 2:
    print_usage()
    exit()

comm = sys.argv[1]
if comm == 'build':
    copyFromSrc()
    merge()

elif comm == 'split':
    split()

elif comm == 'apply':
    copyToSrc()

else:
    print_usage()
