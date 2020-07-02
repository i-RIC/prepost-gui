"""
Remove type="unfinished" from *.ts
"""

newlines = list()
with open('../languages/iRIC_pt_BR.ts', 'r', encoding='utf-8') as f:
    try:
        for l in f.readlines():
            l = l.replace(' type="unfinished"', '')
            l = l.replace('<translation></translation>', '<translation type="unfinished"></translation>')
            newlines.append(l)
    except Exception as e:
        print(e)
        exit()

with open('../languages/iRIC_pt_BR.ts', 'w', encoding='utf-8') as f:
    f.write("".join(newlines))

