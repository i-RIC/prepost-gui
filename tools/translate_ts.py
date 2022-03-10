# ------------------------------------------------------------
# Read me
# 
# 本プログラムで行う処理
# tsファイル（xml形式）内の未翻訳の文章を翻訳する
# 
# Input data -> 〇〇.ts（プログラムではフォルダを指定している）
# Output data -> 〇〇.ts（上書き保存される）
#
# Processing performed by this program
# Translate untranslated text in ts file (xml format)
#
# Input data -> 〇〇.ts（The program specifies a folder）
# Output data -> 〇〇.ts（Overwritten and saved）
# ------------------------------------------------------------

import html
import os
import requests
import glob
import re
from google.cloud import translate_v3 as translate
import xml.etree.ElementTree as ET

def translation_by_deepl(word, lang):
    # API_URL = 'https://api.deepl.com/v2/usage'
    API_URL = 'https://api.deepl.com/v2/translate'
    query = {
        'auth_key' : '1cb853a0-3ce8-45e7-aafa-783a7211c538',
        'text' : word,
        'target_lang' : lang
    }
    res = requests.get(API_URL, params=query)
    # print(res.json())
    translated_str = res.json()['translations'][0]
    # print(translated_str['text'])
    return translated_str['text']

def translation_by_googlecloud(word, lang):

    # 作業フォルダの取得
    # Get working folder
    cwd = os.getcwd()

    # GCPのプロジェクトIDを読み込む
    # Read the GCP project ID
    project_id = "iric-ts-translation"
    # Google Cloud Translationアクセス用のkeyを読み込む
    # Load the key for Google Cloud Translation access
    ##key = cwd + '/iric-ts-translation-4761fe6f85d6.json'
    # 上記アクセスkeyを環境変数に設定する
    # Set the above access key to an environment variable
    ##os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = key

    # 以下は特に変更しない
    # The following is not changed
    client = translate.TranslationServiceClient()
    location = "global"
    parent = f"projects/{project_id}/locations/{location}"

    # 以下の情報をGoogle Cloud Translationに投げる
    # Send the following information to Google Cloud Translation
    # print(word)
    word, placeholders = make_placeholders(word)

    response = client.translate_text(
        request={
            "parent": parent,
            "contents": [word],
            "mime_type": "text/html",
            "source_language_code": "en",
            "target_language_code": lang,
        }
    )

    # Display the translation for each input text provided
    for tl in response.translations:
        trans = tl.translated_text
        trans = apply_placeholders(trans, placeholders)
        trans = cleanup_trans(trans)
        # print(trans)
        break
    
    return trans

def make_placeholders(word):
    word = html.escape(word)
    placeholders = dict()

    # メニュー対応
    # Menu support

    m = re.search(r'^(.*)(&amp;(.))(.*?)(\.\.\.|)$', word)
    if not m is None:
        # print(m.groups())
        s1, s2, s3, s4, s5 = m.groups()
        v = '(&amp;' + s3.upper() + ')'
        placeholder = '<span id="{0}"></span>'.format(len(placeholders) + 1)
        word = '{0}{1}{2} {3}{4}'.format(s1, s3, s4, placeholder, s5)
        placeholders[placeholder] = v

    patterns = ['(%[0-9]+)', '(\(\*\..+\))']

    while True:
        found = False
        for p in patterns:
            m = re.search(p, word)
            if not m is None:
                v = m.groups()[0]
                placeholder = '<span id="{0}"></span>'.format(len(placeholders) + 1)
                word = word.replace(v, placeholder)
                placeholders[placeholder] = v
                found = True
        
        if found: continue

        break

    return word, placeholders

def cleanup_trans(trans):
    trans2 = _cleanup_trans(trans)
    while trans != trans2:
        trans = trans2
        trans2 = _cleanup_trans(trans)
    
    return trans2

def _cleanup_trans(trans):
    trans = trans.replace('  ', ' ')
    trans = trans.replace(' .', '.')
    trans = trans.replace(' ,', ',')

    # (%1), "%1" に対応
    # Corresponds to (%1), "%1"
    
    m = re.search(r'^(.*)\( (%[0-9]) \)(.*)$', trans)
    if not m is None:
        groups = m.groups()
        trans = groups[0] + "(" + groups[1] + ")" + groups[2]

    m = re.search(r'^(.*)" (%[0-9]) "(.*)$', trans)
    if not m is None:
        groups = m.groups()
        trans = groups[0] + '"' + groups[1] + '"' + groups[2]

    return trans

def apply_placeholders(word, placeholders):
    # print(word)
    for k, v in placeholders.items():
        word = word.replace(k, ' ' + v + ' ')

    return html.unescape(word.strip())

def translation(word, lang, select_API):

    DeepL_dict = {'bg_BG':'BG', 'cs_CZ':'CS', 'da_DK':'DA', 'de_DE':'DE',
                  'el_GR':'EL', 'en_  ':'EN', 'es_ES':'ES', 'et_EE':'ET',
                  'fi_FI':'FI', 'fr_FR':'FR', 'hu_HU':'HU', 'it_IT':'IT',
                  'ja_JP':'JA', 'lt_LT':'LT', 'lv_LV':'LV', 'nl_NL':'NL',
                  'pl_PL':'PL', 'pt_PT':'PT', 'ro_RO':'RO', 'ru_RU':'RU',
                  'sk_  ':'SK', 'sl_SI':'SL', 'sv_SE':'SV', 'zh_CN':'ZH'}

    google_dict = {'ar_EG':'ar', 'bg_BG':'bg', 'bs_BA':'bs', 'ca_ES':'ca',
                   'cs_CZ':'cs', 'da_DK':'da', 'de_DE':'de', 'el_GR':'el',
                   'es_ES':'es', 'et_EE':'et', 'eu_ES':'eu', 'fi_FI':'fi',
                   'fr_FR':'fr', 'gl_ES':'gl', 'hi_IN':'hi', 'hu_HU':'hu',
                   'id_ID':'id', 'is_IS':'is', 'it_IT':'it', 'ja_JP':'ja',
                   'ko_KR':'ko', 'ky_KG':'ky', 'lt_LT':'lt', 'lv_LV':'lv',
                   'nb_NO':'no', 'nl_NL':'nl', 'pl_PL':'pl', 'pt_BR':'pt',
                   'pt_PT':'pt', 'ro_RO':'ro', 'ru_RU':'ru', 'sl_SI':'sl',
                   'sv_SE':'sv', 'th_TH':'th', 'tr_TR':'tr', 'uk_UA':'uk',
                   'vi_VN':'vi', 'zh_CN':'zh-cn', 'zh_TW':'zh-tw'}

    # 翻訳できない言語の場合、Errorと表記させる
    # If the language cannot be translated, write it as Error.
    if lang in google_dict == False:
        print(lang + "には翻訳できません")
        trans = 'Error'
        return trans

    if select_API == 1:
        # 翻訳したい言語にDeepLが対応しているかチェック
        # Check if DeepL supports the language you want to translate
        judge = lang in DeepL_dict

        if judge == True:
            # deepl辞書からlangを取得する
            # Get lang from deepl dictionary
            lang = DeepL_dict[lang]
            # DeepLにwordとlangを投げる
            # Send word and lang to DeepL
            trans = translation_by_deepl(word, lang)
        
        else:
            # google辞書からlangを取得する
            # Get lang from google dictionary
            lang = google_dict[lang]
            # google翻訳にwordとlangを投げる
            # Send word and lang to google translate
            trans = translation_by_googlecloud(word, lang)
    
    else:
        # google辞書からlangを取得する
        # Get lang from google dictionary
        lang = google_dict[lang]
        # ここでgoogle翻訳にwordとlangを投げる
        # Send word and lang to google translate here
        trans = translation_by_googlecloud(word, lang)

    return trans

def main(src_folder, tgt_folder, select_API):

    # tsファイルリストの取得
    # Get ts file list
    file_list = glob.glob(src_folder + '/*.ts')

    for fl in file_list:
        tgt_name = fl.replace(src_folder, tgt_folder)

        if os.path.exists(tgt_name):
            continue

        print(fl)

        # tsファイルをparseする
        # parse ts file
        tree = ET.parse(fl)
        root = tree.getroot()

        # 翻訳したい言語を取得
        # Get the language you want to translate
        lang = root.attrib['language']
        print(f"lang = {lang}")

        # contextタグのリストを取得
        # Get a list of context tags
        con_list = root.findall('context')

        # context内に含まれるmessageタグのリストを取得
        # Get a list of message tags contained in context
        for cl in con_list:
            mes_list = cl.findall('message')

            for ml in mes_list:

                # translationタグに属性がない場合、何もしない
                # If the #translation tag has no attributes, do nothing
                if ml[1].attrib == {}:
                    # print('OK')
                    pass

                # message内のtranslationのtypeが'unfinished'かどうか判断
                # Determine if the translation type in the message is'unfinished'
                elif ml[1].attrib['type'] == 'unfinished':

                    # 翻訳したいテキストを取得
                    # Get the text you want to translate
                    before_text = ml[0].text

                    # 翻訳処理
                    # Translation process
                    trans_text = translation(before_text, lang, select_API)
                    # print(trans_text)

                    # ml[1]のunfinished属性を消す
                    # Remove the unfinished attribute of ml [1]
                    ml[1].attrib.pop('type', None)

                    # ml[1]のテキストにtrans_textを代入する
                    # Substitute trans_text for the text of ml [1]
                    ml[1].text = trans_text
                
                # vanished
                elif ml[1].attrib['type'] == 'vanished':
                    # 何もしない
                    # do nothing
                    pass

                # obsolete
                elif ml[1].attrib['type'] == 'obsolete':
                    # 何もしない
                    # do nothing
                    pass
                else:
                    print('属性情報が不正です: {0}'.format(ml[1].attrib['type']))    # The attribute information is invalid
                    print('ファイル => ' + fl)                                      # File

        tree.write(tgt_name, encoding='UTF-8')

    return 0

if __name__ == '__main__':

    # 翻訳したいtsファイルを格納するフォルダ
    # Folder to store the ts file you want to translate
    folder = './languages'

    # 翻訳APIの選択
    # DeepLで翻訳できる言語はDeepLを使う => 1
    # 全ての言語でgoogle Cloud Translationを使う => 2
    # Translation API selection
    # Use DeepL for languages ​​that can be translated with DeepL => 1
    # Use google Cloud Translation in all languages ​​=> 2
    select_API = 2

    main('input', 'output', select_API)
