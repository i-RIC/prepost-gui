import iricdevtool as idev
import subprocess
import os
import re

def update_translation(path):
  langs = [
    'ar_EG',  # Arabic
    'bg_BG',  # Bulgarian
    'bs_BA',  # Bosnian
    'ca_ES',  # Catalan
    'cs_CZ',  # Czech
    'da_DK',  # Danish
    'de_DE',  # German
    'el_GR',  # Greek
    'es_ES',  # Spanish
    'et_EE',  # Estonian
    'eu_ES',  # Basque
    'fi_FI',  # Finnish
    'fr_FR',  # French
    'gl_ES',  # Galician
    'hi_IN',  # Hindi
    'hu_HU',  # Hungarian
    'id_ID',  # Indonesian
    'is_IS',  # Icelandic
    'it_IT',  # Italian
    'ja_JP',  # Japanese
    'ko_KR',  # Korean
    'ky_KG',  # Kyrgyz
    'lt_LT',  # Lithuanian
    'lv_LV',  # Latvian
    'nb_NO',  # Norwegian
    'nl_NL',  # Dutch
    'pl_PL',  # Polish
    'pt_BR',  # Portuguese (Brazil)
    'pt_PT',  # Portuguese (Portugal)
    'ro_RO',  # Romanian
    'ru_RU',  # Russian
    'sl_SI',  # Slovenian
    'sv_SE',  # Swedish
    'th_TH',  # Thai
    'tr_TR',  # Turkish
    'uk_UA',  # Ukrainian
    'vi_VN',  # Vietnamese
    'zh_CN',  # Chinese (簡体字)
    'zh_TW'   # Chinese (繁体字)
  ]

  print(path)
  p = re.compile(r"\/(.*?)_.+\.ts")
  with open(path, 'r', encoding='utf-8') as f:
    new_lines = list()
    prefix = ""
    for l in f.readlines():
      if '.ts' in l:
        m = p.search(l)
        prefix = m.groups()[0]
      else:
        new_lines.append(l)

  if prefix:
    for i, l in enumerate(langs):
      if i == 0:
        new_lines.append("TRANSLATIONS += languages/" + prefix + "_" + l + ".ts \\\n")
      elif i == len(langs) - 1:
        new_lines.append("                languages/" + prefix + "_" + l + ".ts\n")
      else:
        new_lines.append("                languages/" + prefix + "_" + l + ".ts \\\n")

  with open(path, 'w', encoding='utf-8') as f:
    f.write("".join(new_lines))

print('updating *.pro to add translations...')
idev.recursiveExec("..", r"\.pro$", update_translation)
