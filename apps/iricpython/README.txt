iRIC Python ビルド方法

build.bat を実行 --> _iric.pyd, iric.py が生成
iric.py は、list から IntVector への詰め替えに関する
変更を加えてあるので、 Subversion に保存されているものを利用する。

_iric.pyd, iric.py を、以下のフォルダにコピーする。

E:\Python35\Lib\site-packages

また、iRIC 関連の各種 DLL にパスが通っている必要がある。通常以下に
iRIC がインストールされているので、ここにパスを通す。

C:\Users\(ユーザ名)\iRIC\guis\prepost

Python を起動し、 import iric とすると、 iric モジュールがインクルードされる
