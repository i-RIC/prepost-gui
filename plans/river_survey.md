# 横断測量データ 断面表示画面 バグ調査・回帰テスト整備プラン

- 作成日: 2026-10-20
- 対象: `GeoDataRiverSurveyCrosssectionWindow`（横断測量データの断面表示画面）と関連クラス
- 状態: **Phase 1〜4 実施済み（2026-10-20）**。結果は §12 を参照。残りは Phase 5（人間による手動テスト、§8.2）と、§12.6 の未対応項目。依頼事項 R-1〜R-3 と R-6 は回答済み。

---

## 1. 目的とスコープ

最近、断面表示画面まわりでバグ報告が続いている（下記の修正コミットを参照）。そこで静的解析とコードレビューで網羅的にバグを洗い出し、修正する。あわせて、同じ不具合が再発しないよう**繰り返し実行できる自動回帰テスト**を整える。

**追加スコープ（2026-10-20）**: ユーザーから「横断測量データを一度エクスポートしてからインポートすると、場所がずれる」という報告があった。この現象の再現確認と修正も対象に加える（§2.1、§7.3）。

最近のバグ修正履歴（回帰テストの対象にする）:

| コミット | 内容 |
|---|---|
| `113e2ffba` | 河川横断線に点を追加すると異常終了する #1570（`EditCrosssectionCommand` の index 計算） |
| `d0671f51d` | クラッシュするバグを修正（GraphicsView の植生 (jmk) 処理でのイテレータ／参照の無効化） |
| `cff35e30d` | 同じ断面とみなす条件を変更（断面名の数値比較） |
| `124a905f8` | 左岸からの距離を編集したときの位置計算 |
| `63587e7da` | 中洲掘削（SlopePointEditDialog）の不具合修正 |
| `e8dd97055` / `59eccddb0` | 面積計算ダイアログ関連 |

### 対象ファイル（`libs/geodata/riversurvey/` 配下）

| 区分 | ファイル |
|---|---|
| メイン画面 | `geodatariversurveycrosssectionwindow.{h,cpp,ui}`, `private/geodatariversurveycrosssectionwindow_impl.h` |
| 描画・マウス操作 | `geodatariversurveycrosssectionwindowgraphicsview.{h,cpp}`（約 2,300 行。最もリスクが高い） |
| テーブル／デリゲート | `private/geodatariversurveycrosssectionwindow_{datatable,vegetationdatatable,riversurveytable,wsetable}delegate.*`, `geodatariversurveycrosssectionwindow{tableview,vegetationtableview,wsetablewidget}.*` |
| ダイアログ | `geodatariversurveycrosssectioneditfrompointdialog.*`, `geodatariversurveycrosssectionslopepointeditdialog.*`, `geodatariversurveycrosssectiondisplaysettingdialog.*`, `geodatarivercrosssectionaltitudemovedialog.*`, `private/..._pointadddialog.*`, `private/..._jmkdataeditdialog.*`, `private/geodatariversurvey_calcareaconditiondialog.*` |
| Undo コマンド | `private/geodatariversurvey_{editcrosssection,mouseeditcrosssection,editjmkdata,editjmkdatabydrag,editslopepoint,setodnpoint,expandcrosssection,rotaterivercrosssection,mouserotaterivercrosssection}command.*`, `private/..._graphicsview_setdisplaysettingcommand.*` |
| データモデル | `geodatarivercrosssection.*`, `geodatariverpathpoint.*`, `geodatariverpathpointodndata.*`, `geodatariverpathpointjmkdata.*` |
| 呼び出し側 | `libs/pre/datamodel/preprocessorgeodatagroupdataitem.cpp`（`openCrossSectionWindow`、インポート時のオフセット適用など） |
| エクスポート／インポート（追加） | `geodatariversurveyexporter.cpp`（*.riv と中心点の *.riv.csv）、`geodatariversurveyimporter.cpp`、`geodatariversurveyimportersettingdialog.*`、`private/geodatariversurveyimporter_importersetting.*`、`geodatariversurveyodn{exporter,importer}.cpp`、`geodatariversurveyhonryuexporter.cpp`、`geodatariversurveymlitimporter.cpp`、`geodatariverpathpoint.cpp`（`shiftCenter`, `crosssectionPosition`）、`GeoDataRiverSurvey::doApplyOffset` |

---

## 2. 予備調査の結果（バグ候補）

`geodatariversurveycrosssectionwindow.cpp` の全体と、`graphicsview.cpp`・`editcrosssectioncommand.cpp`・`geodatarivercrosssection.cpp` の一部を読んで見つけた候補です。**どれも未確定**で、Phase 1 で精査し、Phase 3 のテストで再現を確認してから修正します。

重大度: **H** = クラッシュ／データ破壊、**M** = 誤動作・特定条件でのクラッシュ、**L** = 軽微・保守性

| ID | 重大度 | 場所 | 内容 | 再現の想定 |
|---|---|---|---|---|
| B-01 | H | `Window::deleteSelectedRows` → `GeoDataRiverCrosssection::removePoint(const std::vector<int>&)` | `removePoint` は index が**昇順**であることを前提に offset を引いて erase している。しかし `selectedRows()` の順序は選択した順で、昇順とは限らない。Ctrl+クリックで後ろの行から選ぶと、違う点が消えるか、`begin()-1` を erase して範囲外アクセスになる | 4 行目→1 行目の順に Ctrl+クリックして削除 |
| B-02 | H | `Window::deleteSelectedVegetation` | B-01 と同じ。`ids` をソートせずに逆順で erase しているので、違う植生が消えるか範囲外アクセスになる | 植生テーブルで 3 行目→1 行目の順に選んで削除 |
| B-03 | H | `GraphicsView::editCrossSection`（マウスによる断面編集。左側） | 左方向の erase 範囲が 1 つずれている。ループは `position <= alt` の点で止まるが、その点も `[eraseBegin, eraseEnd)` に含まれて**残すべき点が 1 つ消える**。右側と、ダイアログ版 (`EditFromPointDialog`) の左側は正しい挙動なので、仕様の差ではなく不具合とみられる | 点 [0,1,2,3,4] で 3 を選び、x=1.5 をクリックすると 1 も消える |
| B-04 | H | `EditCrosssectionCommand` コンストラクタ | (a) `std::lower_bound` の結果が `end()` のときにそれを参照外ししている（`*it`）。例: ODN が未設定 (-1) のまま右端の点を削除。(b) 範囲チェックは `posvec`（after）の size で行っているのに、`before.at(index)` で before を参照している。before が空なら `at(-1)` になる | ODN 未設定の断面で右端の点を削除（Debug ビルドならアサートで落ちる） |
| B-05 | M | `EditCrosssectionCommand` コンストラクタ | ODN が**未設定 (-1)** でも、編集するたびに 0 または末尾 index が入り、**勝手に設定済みになる**（表の背景色やマーカーが出る）。仕様の確認が必要 | ODN 未設定の断面で標高値を 1 つ編集する |
| B-06 | H | Undo コマンド全般（`EditCrosssectionCommand`, `EditJmkDataCommand`, `EditSlopePointCommand`, `MouseEditCrosssectionCommand`, `SetDisplaySettingCommand` など） | 断面ウィンドウ（または GraphicsView）の**生ポインタ**を持っている。ウィンドウを閉じても `iRICUndoStack` はクリアされないので、閉じた後に Undo/Redo すると**解放済みメモリへのアクセス**になる疑いがある | 断面ウィンドウで点をドラッグ → ウィンドウを閉じる → Ctrl+Z |
| B-07 | H | `Window::handleWseTableItemEdit`, `inactivateByWEOnlyThis`, `deleteSelectedRows`, `setSelectedPointToOdn*`, `editFromSelectedPointWithDialog` など | `m_editTargetPoint` が nullptr のとき（対象の測量データのチェックを外したとき／その断面名を持たないとき／格子生成モードのとき）に参照外しする | 測量データ表で対象のチェックを外してから水位表の値を編集する、など |
| B-08 | M | `Window::setupData` / `updateActionStatus` | target が nullptr だと早期 return するので、`m_settingUp` が **true のまま**になり、`updateActionStatus()` も呼ばれない。行を削除しても `selectionChanged` は通知されないので、「削除」「ダイアログで編集」アクションが**有効のまま**残る。そこで実行すると `rows.at(0)` や nullptr の参照外しになる | 行を選択 → 対象のチェックを外す → メニューから「削除」 |
| B-09 | M | `setupCrosssectionNames` と `isSameCrosssectionName` | コンボボックスは**文字列の完全一致**で重複を除いているのに、点の検索は**数値の許容誤差**で一致を判定している。"100" と "100.0" が両方コンボボックスに出て、どちらを選んでも同じ点になる | 2 つの測量データで断面名が "100" と "100.0" |
| B-10 | M | `updateWaterSurfaceElevationTable` と `handleWseTableItemEdit` | 水位表の表示は `crosssectionName()` をキーにしているが、編集は `m_editTargetPoint->name()` をキーにしている。B-09 の状況では、**表示している項目と違う項目を書き換える** | B-09 と同じデータで水位を編集 |
| B-11 | M | `Window::referenceRiverPathPoint` / `referenceRiverPathPointColor` | 参照断面のチェックが ON でコンボボックスが空 (index -1) のとき `at(-1)` になる。測線を削除した後は、`m_referenceRiverPathPoints` に削除済みの点が残っていないかも確認する | 参照断面 ON の状態で測線を削除・追加し、Undo/Redo する |
| B-12 | M | `GeoDataRiverCrosssection::removePoint(vector)` | 点を削除しても `m_fixedPointL/R`（固定点の index）を詰め直していない（格子生成の条件と関係する） | 固定点を設定した断面で、固定点より左の点を削除 |
| B-13 | L | `Window::syncData` | 行数が 1 のとき `(it + i + 1)` が範囲外になる。モデルの行数と `old.size()` が一致しないときの防御もない | 点が 1 つしかない断面を読み込む |
| B-14 | L | `Window::crosssectionComboBoxChange` | `newindex == -1` だと `at(-1)` になる（シグナルをブロックしていない経路があるか確認する） | — |
| B-15 | L | `Window::setupActions` | `m_leftAddAction` などの 4 つは親なしで `new` しているので、メモリリークする | — |
| B-16 | M | `GraphicsView::createAltitude`, `enterEditCrosssectionMode`, `editCrossSection` | `selectedIndexes().at(0)` を使っており、選択が空になると落ちる。編集モード中に Undo やテーブル操作で選択が消える場合が該当する | 断面編集モードに入る → テーブルで選択を解除 → マウスを動かす |

### 2.1 エクスポート → インポートで位置がずれる問題（ユーザー報告）

**報告の内容（R-6 の回答、2026-10-20）**:

- 形式は **\*.riv**。
- ずれ方: **断面表示画面で、断面図が横に移動していた**。
- 元のファイル一式は残っていない。そこで、`check.ipro`（R-1）から *.riv をエクスポートし、同時に書き出される *.riv.csv と一緒にインポートして再現を試す。
- **中心点を \*.csv から決める設定でずれるか**を確認する。CSV を使わない設定（中点・最低河床）でずれるのは**仕様**なので対象外。

#### .riv の往復で何が書き出され、何が再計算されるか（現状）

| 項目 | エクスポート | インポート |
|---|---|---|
| 左岸・右岸の座標 | **有効な**左岸点・右岸点の座標（`crosssectionPosition`）＋プロジェクトのオフセット | その 2 点を結ぶ**直線**を断面の方向とする |
| 断面の点 | **有効な点だけ**を書き出す。距離は「有効な左岸点からの距離」 | 先頭の点を距離 0（= 左岸座標）として並べる |
| 中心点（測線の位置） | `<ファイル名>.riv.csv` に書き出す | 設定ダイアログの「中心点」の選択で決まる（既定は「左右岸の中点」）。**今回の対象は「CSV」を選んだ場合** |
| 低水路の 4 点（`divIndices`） | 書き出さない | 「4 点あり」かどうかで、「中点」の意味（低水路の中点か、左右岸の中点か）が変わる |
| 延長線（固定点と折れた方向 `DirectionL/R`） | 折れた先の座標をそのまま左岸・右岸として書き出す | 直線として扱う |
| 左岸からの距離の基準（`leftShift`） | 書き出さない | 「先頭の点から中心点まで」で再計算する |
| 断面の並び順 | 現在の順序 | 名前がすべて数値なら **KP の降順に並べ替える** |

#### 報告された症状と、最も疑わしい原因（E-04）

断面表示画面の横軸は「中心からの距離」ではなく、**「左岸からの距離」＝ `position + leftShift`** で描かれている（`GraphicsView` の描画処理は、すべて `alt.position() + leftShift` で座標を変換している。中央の破線も `x = leftShift` に描いている）。

CSV を使う設定で取り込むと、中心点そのものは正しく戻る（直線の断面なら、中心点は左右岸を結ぶ線の上にあるため）。しかし `leftShift` は「エクスポートした左岸点（＝**有効な点のうち最も左の点**）から中心点までの距離」に置き換わる。その結果、断面図全体が次の量だけ横に移動する。

> **ずれ量 = 元の「最も左の有効な点」の左岸からの距離 = `pos_first_active + leftShift_old`**

元の最も左の有効な点が、左岸からの距離 0 の位置になかったときに、このずれが起きる。具体的には次のような場合である。

- 元のデータで、先頭の点が距離標から離れていた（国交省形式などで多い）。
- 4 点ありで取り込み、低水路の 4 点の外側の点が除かれていた。
- 左端の点を無効にしていた（E-05）。
- 左に延長線を追加していた（延長の点は左岸からの距離が負になる。E-03）。

`check.ipro` の 2 件の測量データを重ねて表示すると、片方だけを往復させた場合に横方向のずれがはっきり見える。報告の症状と一致する。

#### 原因の候補（R-6 の回答を受けて見直した。CSV を使う設定が前提）

| ID | 重大度 | 場所 | 内容 | 想定される「ずれ」 |
|---|---|---|---|---|
| E-04 | **H（最有力）** | Exporter（距離の基準） | 距離を「有効な左岸点からの距離」で書き出しており、元の基準（`leftShift`）が保存されない。断面図は `position + leftShift` で描かれるので、断面図が**横に移動する** | 断面表示画面で断面図が横に移動する（**報告の症状と一致**） |
| E-03 | H | Exporter と `crosssectionPosition`（延長線） | 延長線を追加した断面は折れ線になっている。両端の座標を直線として読み戻すため、断面の方向が変わる。CSV の中心線との交点も元の中心点からずれる | 断面の回転、中心点のずれ、断面図の横移動 |
| E-05 | M | Exporter（無効な点） | 無効な点は書き出されない（.riv 形式に有効フラグがないため）。左端の点が無効だと、E-04 のずれ量が大きくなる | 断面図の横移動。無効な点が消える |
| E-08 | M | Importer `findCrossSection` | 中心線と最初に交わった区間を採用しており、CSV の name 列を使っていない。蛇行区間で断面線が中心線と複数回交わると、別の交点を中心点にしてしまう | 一部の断面だけ中心点がずれる |
| E-09 | L | Exporter（数値の精度） | 有効桁数 10 桁の `SmartNotation`。大きな座標＋オフセットの場合にミリ単位の丸めが出ないか確かめる | ミリ単位の丸め |
| E-10 | 要確認 | オフセットの扱い | エクスポートで加え、インポート後に `applyOffset` で引いている。対応は正しく見えるが、テストで確かめる | データ全体の平行移動 |
| E-06 | M | Importer の並び替え | 名前がすべて数値だと KP の降順に並べ替える。`doInitWithSetting` では並べ替えを行わず、`m_reverseOrder` にダイアログの値が入らない（位置のずれとは別の問題） | 上下流の逆転 |
| E-07 | M | Importer `readRivFile` / `getPoint` | 空白を含む断面名を誤読する。"100" と "100.0" が 1 つの断面に統合される（位置のずれとは別の問題） | 座標の誤読、断面の欠落 |
| E-01 | 仕様（対象外） | Importer の中心点の設定 | 「中点」「最低河床」で取り込むと中心点が変わる。R-6 の回答により**仕様** | — |
| E-02 | 仕様（対象外） | 低水路の 4 点の情報 | 4 点の情報が消え、「中点」の意味が変わる。CSV を使う設定なら中心点には影響しない。制御点 (`CenterToLeft/RightCtrlPoints`) が消える点は別に記録しておく | — |
| E-11 | 対象外 | ODN と HONRYU の形式 | 報告は *.riv なので該当しない | — |

---

## 3. 進め方（全体の流れ）

```
Phase 0 準備 ─> Phase 1 静的解析＋レビュー ─> Phase 2 テスト基盤 ─> Phase 3 再現テスト作成（Red）
                                                                    └─> Phase 4 修正（Green）─> Phase 5 手動テスト（人間）─> Phase 6 報告
```

- バグは **1 件 = 1 テスト = 1 修正コミット** で進める。先に失敗するテストを書いてから直す。
- 修正は最小限にする。B-03 や B-05 のように仕様の判断が必要なものは、修正前に確認をお願いする。
- **エクスポート → インポートでずれる問題（E 系）** は、次の順で別に進める。ほかの項目とは独立しているので、Phase 2 の基盤ができた時点で並行して着手できる。
  1. 報告の内容は確認済み（§9 の R-6）。形式は *.riv、症状は「断面表示画面で断面図が横に移動する」、対象は CSV を使う設定
  2. `check.ipro` で再現する（§7.3 の RT-13）。測ったずれ量が、E-04 の予測式 `pos_first_active + leftShift_old` と一致するかを確かめる
  3. 合成データの往復テスト（RT-01〜RT-12）で、ほかの原因（E-03, E-05, E-08 など）も切り分ける
  4. 往復で何を保存すべきか決め（§10 の D-7〜D-9）、修正する。往復テストは回帰テストとして残す

---

## 4. Phase 0: 準備

1. ビルド環境の確認（確認済み）: VS2019 Professional 16.11（v142）、Qt 5.14.2 msvc2017_64（`Qt5Test.lib` あり）、`libdlls/Debug/iricGdRiversurvey.dll` はビルド済み。
2. 静的解析ツールの準備:
   - **MSVC Code Analysis (`/analyze`)**: 追加インストールなしで使える。
   - **cppcheck**: 未インストール。winget の `Cppcheck.Cppcheck` か portable zip で入れる。**インストールには承認が必要**。
   - **clang-tidy**: 任意。`pip install clang-tidy` で入れ、include パスは vcxproj から抽出する。効果に対して手間が大きいので、cppcheck で足りなければ使う。
3. ベースラインとして、現状の Debug ビルドが通ることを確認する。

---

## 5. Phase 1: 静的解析とコードレビュー

### 5.1 ツールによる解析

| ツール | 実行方法 | 対象 |
|---|---|---|
| MSVC `/analyze` | `msbuild libs\geodata\riversurvey\iricGdRiversurvey.vcxproj /p:Configuration=Debug /p:Platform=x64 /p:RunCodeAnalysis=true /p:EnablePREfast=true` | riversurvey ライブラリ全体。C6xxx（nullptr 参照外し、範囲外アクセス）と C26xxx（寿命）の警告を、対象ファイルだけに絞って確認する |
| cppcheck | `cppcheck --enable=warning,style,performance --std=c++14 --library=qt -I libs libs\geodata\riversurvey` | 同上 |

ツールの警告はそのまま採用せず、全件を目で確認して**誤検知を除いた結果**だけを §2 の表に追加する。

### 5.2 観点別の手動レビュー（チェックリスト）

ツールでは見つけにくいので、次の観点でファイル単位に精読する。順序はリスクの高い順で、graphicsview.cpp → window.cpp → commands → dialogs → delegates → datamodel とする。

1. **nullptr のガード漏れ**: `target()` / `m_editTargetPoint` / `m_targetRiverSurvey` / `waterElevation()` / `hydraulicDataGroupDataItem()` の戻り値
2. **index の範囲**: `at(0)`、`[row]`、`it + i ± 1`、`selectedIndexes().at(0)`、`currentIndex() == -1`
3. **選択の順序に依存する処理**: `selectedRows()` や `selectedIndexes()` の結果が昇順だと仮定しているか（B-01, B-02）
4. **コンテナの変更中の参照・イテレータの無効化**: `d0671f51d` と同じパターン（参照を保持したまま Undo の push → `updateCrossectionWindows()` で再構築される）
5. **生ポインタの寿命**: Undo コマンドが持つ window / view / point、`m_referenceRiverPathPoints`、モードレスダイアログが持つ parent（B-06, B-11）
6. **再入と状態フラグ**: `m_settingUp` の true/false の対応、`blockSignals` の漏れ、`dataChanged` → `syncData` → `setData` → `dataChanged` の再帰
7. **モードの遷移**: `MouseEventMode` の全遷移で、Esc・ダイアログの破棄・ウィンドウを閉じる・Undo・断面の切り替えをしたときに `meNormal` へ戻るか
8. **キーの一貫性**: 断面名の比較（完全一致と数値一致の混在）、水位データのキー（B-09, B-10）
9. **Undo/Redo の対称性**: `redo()` と `undo()` が同じものを更新しているか（ODN、選択、`setMapped`）、`mergeWith` の条件
10. **例外**: `GeoDataRiverCrosssection` が投げる `ErrorCodes` を呼び出し側がすべて catch しているか（`leftBankIndex` の `ec_NoAltitudeExists` など）
11. **浮動小数点の比較**: `==` / `!=` による位置の比較（`syncData`, `EditCrosssectionCommand`）

### 5.3 成果物

- §2 の表を更新する（確定・誤検知・仕様確認待ちに分類）。
- 確定したバグについて、それぞれ再現手順・原因・修正方針をこのファイルに追記する。

---

## 6. Phase 2: 自動テスト基盤

### 6.1 方針: 2 層構成

| 層 | 目的 | 実行時間 | 必要なもの |
|---|---|---|---|
| **A. ロジックの単体テスト** | データモデルとコマンドの計算ロジックを、GUI を起動せずに高速に検証する | 数秒 | `iricGdRiversurvey.dll` と依存 DLL |
| **B. GUI 統合テスト** | 実際の `iRICMainWindow` をプロセス内で起動して断面ウィンドウを開き、QTest でキーやマウスの操作を送り、モデル・Undo・クラッシュを検証する | 数十秒〜数分 | デスクトップのセッション（VTK が OpenGL を使うので、ヘッドレス実行は不可）、ソルバー定義 |

テストフレームワークには **QtTest**（Qt 5.14.2 に付属）を使う。既存の `tests/geometry` と `tests/iricpython` は単純な exe でアサーションがないので、QtTest で新しく作る。

### 6.2 プロジェクト構成（案）

```
tests/
  riversurvey_logic/                 # 層 A
    riversurvey_logic.pro
    riversurveyLogicTest.vcxproj      # qmake -tp vc で生成し、tests.sln に追加
    tst_geodatarivercrosssection.cpp
    tst_editcrosssectioncommand_odn.cpp
    tst_crosssectionnames.cpp
    tst_crosssectionediterase.cpp
  riversurvey_crosssectionwindow/    # 層 B
    riversurvey_crosssectionwindow.pro
    riversurveyCrosssectionWindowTest.vcxproj
    tst_crosssectionwindow.cpp
    tst_riversurveyroundtrip.cpp      # エクスポート→インポートの往復テスト（§7.3）
    helpers/
      modaldialogresponder.{h,cpp}    # 自動でモーダルダイアログに応答する
      testprojectbuilder.{h,cpp}      # 合成データからプロジェクトを作る
      crosssectionwindowdriver.{h,cpp}# 画面の操作をラップする（行の選択、セルの編集、マウス操作など）
    data/
      synthetic_basic.riv             # 合成した横断測量データ（テキスト）
      synthetic_two_surveys_name_variants.riv
      roundtrip/                      # 往復テスト用の合成データ（§7.3 の条件ごとに 1 ファイル）
tools/
  run_riversurvey_tests.ps1           # ビルド・PATH の設定・テスト関数ごとに別プロセスで実行・JUnit XML の出力
```

### 6.3 テストしやすくするための最小限の本体改修（**承認が必要**）

本体の挙動は変えずに、次の改修だけを行う。

1. **純粋なロジックの切り出し**: 次の処理を `private/geodatariversurveycrosssectionwindow_logic.{h,cpp}`（`GD_RIVERSURVEY_EXPORT` 付きの名前空間関数）に移し、本体からはそれを呼ぶ。
   - `setupCrosssectionNames` / `isSameCrosssectionName`（現在は無名名前空間にある）
   - `EditCrosssectionCommand` の ODN を対応付け直す計算
   - `GraphicsView::editCrossSection` の erase 範囲の計算
   - `EditFromPointDialog` の左右の erase 計算（可能な範囲で）
2. **objectName の付与**: ツールバー上の `m_crosssectionComboBox`、`m_referenceComboBox`、各 CheckBox などに `setObjectName()` を付け、テストから `findChild` で取得できるようにする（`.ui` 側の `tableView`、`graphicsView`、`surveysTableWidget`、`wsesTableWidget` はすでに名前がある）。
3. GraphicsView のクラスはエクスポートされていないので、テストは `QAbstractItemView*` として扱い、QTest のマウスイベントで操作する（エクスポートは追加しない）。

### 6.4 GUI 統合テストの基盤の要点

- **起動**: `iRICPython::Application::init()` と同じ手順で `QApplication` と `iRICMainWindow` を生成する。`iRICRootPath` は `apps/iricgui/debug` を指す。
- **プロジェクトの作成**: `iRICsolvers_v4_Nays2DH`（R-2 で決定）で新しいプロジェクトを作り、合成した `.riv` を `GeoDataRiverSurveyImporter` で取り込む。境界条件や異常なデータを試す用途に使う。実データの確認には、提供された `check.ipro` を開いて使う（§6.5）。
- **断面ウィンドウを開く**: `PreProcessorGeoDataGroupDataItem::openCrossSectionWindow(rs, name)` を呼び、`QMdiArea` から `GeoDataRiverSurveyCrosssectionWindow` を取得する。
- **モーダルダイアログ**: `QMessageBox::warning`、`QInputDialog::getItem`、`QColorDialog::getColor`、`JmkDataEditDialog::exec`、`InformationDialog` があるとテストが止まる。そこで `ModalDialogAutoResponder` を用意する。`QTimer` で `QApplication::activeModalWidget()` を監視し、あらかじめ指定した応答（OK・Cancel・選択肢）を返して、表示された文言を記録する（テスト側で検証できるようにする）。`InformationDialog` の「次回から表示しない」は QSettings にあらかじめ設定しておく。
- **クラッシュを他のテストに波及させない**: `run_riversurvey_tests.ps1` は QtTest の `-functions` で関数の一覧を取り、**関数ごとに別プロセスで実行**する。異常終了（終了コードが 0 以外、または 0xC0000005）も「失敗」として記録する。
- **Debug ビルドで実行**: MSVC のチェック付きイテレータ（`_ITERATOR_DEBUG_LEVEL=2`）と Qt の `Q_ASSERT`（`QList::at` の範囲チェック）で、範囲外アクセスを確実に検出するため。
- **（任意）AddressSanitizer**: VS 16.11 の `/fsanitize=address` で、`iricGdRiversurvey.dll` とテストの exe だけをビルドする構成を追加し、B-06 のような解放済みメモリへのアクセスを確実に検出する。作業量が大きいので、B-06 をほかの方法で再現できない場合に限る。
- **結果の出力**: `-o results\<func>.xml,junitxml` で関数ごとに出力し、最後に集計する。

### 6.5 テストデータと実行環境（R-1〜R-3 の回答を反映）

| 項目 | 方針 |
|---|---|
| 実データのプロジェクト | `check.ipro`（R-1）を使う。受領データなので、**リポジトリにはコミットしない**。原本を壊さないよう、テストのたびに一時フォルダへコピーしてから開く。パスは環境変数 `IRIC_RS_TEST_PROJECT` で渡し、未設定ならスクリプトの既定値（R-1 のパス）を使う。パスに日本語と空白が含まれるので、引用符の付け方と文字コードに注意する |
| `check.ipro` の使い道 | 横断測量データが 2 件あるので、測量データ表の切り替え（B-UI-02, B-UI-09）、参照断面（B-UI-15）、2 件の比較・面積計算（B-UI-19）、断面名の照合（B-UI-16）、**ずれ報告の再現（RT-13。最優先）** に使う。最初に、ODN・植生・水位データ・断面名の種類が含まれているかを調べ、足りない条件は合成データで補う |
| 合成データ | 境界条件（1 点だけの断面、無効な点、延長線、空白を含む名前など）を作るために `tests/riversurvey_crosssectionwindow/data/` に置く。こちらはコミットする |
| ソルバー | `iRICsolvers_v4_Nays2DH`（R-2）。`apps/solvers/iRICsolvers_v4_Nays2DH/definition.xml` があることを確認済み。テストの開始時に、`iRICRootPath` からこのソルバーが見えることを確かめ、見えなければ分かりやすいメッセージを出して中断する |
| 実行環境 | **`E:\programs\iRIC\start_vc.bat`**（R-3 の訂正後の回答）に、Qt（`qtenv2.bat`）と VS2019 x64（`vcvarsx86_amd64.bat`）の呼び出しと、依存ライブラリの PATH（expat, gdal, geos, hdf5, libpng, netcdf, poco, proj, Qwt, shapelib, sqlite, tiff, udunits, vtk, yaml-cpp の各 debug bin、`iriclib\debug`、`prepost-gui\libdlls\Debug`）が書かれている。ただし最後の行で **`devenv.exe /useenv` を起動する**ため、そのまま `call` してはいけない。`run_riversurvey_tests.ps1` では、このバッチファイルを読んで `devenv` の行を除いた一時バッチファイルを作る。それを `cmd.exe` で実行して環境を整え、同じプロセスの中でテストの exe を起動する。`apps\iricgui\debug` は PATH に追加する。バッチファイルのパスは、環境変数 `IRIC_START_VC` で上書きできるようにする |

---

## 7. Phase 3: テストケース一覧（初版）

### 7.1 層 A: ロジックの単体テスト

| テスト ID | 対象 | 内容 | 関連 |
|---|---|---|---|
| A-01 | `GeoDataRiverCrosssection::removePoint(vector)` | 昇順・降順・順不同の index で、同じ点が消えること | B-01 |
| A-02 | 同上 | 固定点・端点を削除しようとすると `ec_FixDelete`、2 点未満になると `ec_AltitudesMustExistTwo`、左右の偏りで `ec_AltitudesBiased` を投げること | — |
| A-03 | 同上 | 削除後に固定点の index が正しいこと | B-12 |
| A-04 | `activate(vector, false)` | 固定点・端点・2 点未満・偏りの各エラー | — |
| A-05 | `addPoint(pos, h)` | 同じ位置・先頭・末尾に追加したときの挿入位置 | #1570 |
| A-06 | ODN の対応付け直し | 点の追加・削除・移動、ODN 未設定、右端の削除、before が空のとき | B-04, B-05, #1570 |
| A-07 | `setupCrosssectionNames` / `isSameCrosssectionName` | 数値名・非数値名・混在、"100" と "100.0"、許容誤差 1e-6 の境界 | B-09, `cff35e30d` |
| A-08 | マウス編集の erase 計算 | 左右それぞれで、ちょうど点の位置・点と点の間・全点より外側をクリックしたとき | B-03 |
| A-09 | 植生の複数削除 | 順不同の index で正しく削除されること | B-02 |
| A-10 | `GeoDataRiverPathPointOdnData` | YAML の入出力、範囲外 index のガード | #1548 |

### 7.2 層 B: GUI 統合テスト

| テスト ID | 操作 | 期待結果 | 関連 |
|---|---|---|---|
| B-UI-01 | ウィンドウを開く・閉じるを 10 回繰り返す | クラッシュせず、`m_crosssectionWindows` が空に戻る | — |
| B-UI-02 | 断面のコンボボックスで全断面を順に切り替える | タイトル・テーブルの行数がデータと一致する | — |
| B-UI-03 | テーブルで標高・位置・左岸からの距離を編集 → Undo → Redo | モデルとデータが一致し、隣の点を越えないよう制限される | `124a905f8` |
| B-UI-04 | Ctrl+クリックで降順に複数行を選んで「削除」 | 選んだ行が正しく消える | B-01 |
| B-UI-05 | 植生を降順に複数選んで削除 | 同上 | B-02 |
| B-UI-06 | 「点を追加」モードで点を追加（中央・左端の外・右端の外） | クラッシュせず、ODN が保たれる | #1570, B-04 |
| B-UI-07 | 断面編集モードでマウスクリックによる編集（左右）→ Undo | 消える点が想定どおりで、Undo で元に戻る | B-03, B-16 |
| B-UI-08 | ODN 6 種を設定 → 点の削除・追加 → Undo/Redo | ODN の index が正しい点を指し、未設定のものは未設定のまま | B-04, B-05 |
| B-UI-09 | 測量データのチェックを外して target を nullptr にし、全アクションを実行し、水位表を編集する | クラッシュしない（アクションが無効になるか、ガードで無視される） | B-07, B-08 |
| B-UI-10 | 点をドラッグ → ウィンドウを閉じる → Undo ×N → Redo ×N | クラッシュしない | B-06 |
| B-UI-11 | 植生をドラッグ → 断面を切り替える → Undo | クラッシュせず、植生が元に戻る | `d0671f51d` |
| B-UI-12 | 中洲掘削（Left/Right Add/Sub）のダイアログを開いて閉じる、適用、Undo | 期待どおりの断面になり、モードが `meNormal` に戻る | `63587e7da` |
| B-UI-13 | 「ダイアログで編集」（左右 × 交点／距離） | 想定どおりの点が削除・追加される | — |
| B-UI-14 | 水位の追加・上へ・下へ・削除、「水位で無効化」（この断面のみ／全断面） | 状態とメッセージが正しい | B-07, B-10 |
| B-UI-15 | 参照断面を ON にし、測線を削除してから参照コンボボックスを操作 | クラッシュしない | B-11 |
| B-UI-16 | 2 つの測量データで断面名が "100" と "100.0" の場合 | コンボボックスの項目と水位の編集対象が一致する | B-09, B-10 |
| B-UI-17 | 格子生成モードの ON/OFF を切り替えながら各アクションを実行 | クラッシュしない | B-07 |
| B-UI-18 | カメラ操作（Fit、ズーム、移動、アスペクト比の固定、領域の固定）、表示設定の変更 → Undo | 例外が出ず、値が往復で元に戻る | — |
| B-UI-19 | 面積計算ダイアログを開いて閉じる（比較対象あり／なし） | クラッシュしない | `e8dd97055` |
| B-UI-20 | スナップショット（`snapshot()`） | null でない画像が返る | — |

### 7.3 エクスポート → インポートの往復テスト（ずれ報告の再現と回帰）

`GeoDataRiverSurveyImporter` と `GeoDataRiverSurveyExporter` はエクスポートされていないクラスなので、層 B の基盤（iRIC 本体をテストの中で起動する）から、通常のインポート・エクスポートの経路で呼び出す。インポートの設定ダイアログ（中心点の設定、並び順）は `ModalDialogAutoResponder` で操作するか、設定を渡してダイアログを出さずにインポートする経路（`doInitWithSetting`）を使う。

**手順**: 合成データ X を取り込む → 必要なら加工する（延長線の追加、点の無効化、中心点の移動など） → *.riv で書き出す → 新しい測量データとして再インポートする → 元と比べる。中心点の設定は、**CSV（エクスポートで書き出された \*.riv.csv）を基本とする**。中点・最低河床の設定でのずれは仕様なので（R-6）、合否の判定には使わず、参考として記録するだけにする。

**比較する値**（断面ごと。許容誤差は位置 1 mm、標高 1 mm）:

1. 断面の名前と並び順
2. 中心点の座標（`position()`＋オフセット）
3. 断面の方向（`crosssectionDirection`、延長線があれば `DirectionL/R` も）
4. 有効な各点の**平面座標**（`crosssectionPosition(pos)`＋オフセット）と標高
5. `leftShift`（左岸からの距離の基準）
6. **断面表示画面での横座標**（有効な各点の `position + leftShift`）。報告された症状を直接表す値
7. 点の数（有効・無効の別）

テストは値ごとに判定して、どの値が保存されないかを一覧で出力する。この一覧が E-xx の確定根拠になる。

| テスト ID | 元データの条件 | インポート時の中心点の設定 | 主に確認する候補 |
|---|---|---|---|
| **RT-13** | **`check.ipro` の 2 件の測量データ（報告の再現。最優先）** | CSV | **E-04**、ほか |
| RT-01 | 基本（直線の断面、先頭の点の距離が 0、全点有効） | CSV | 基準（ここではずれないこと） |
| RT-03 | 最低河床の位置で中心点を決めたデータ | CSV（中点は参考） | 中心点が CSV で正しく戻ること |
| RT-04 | 低水路の 4 点ありで取り込んだデータ（4 点の外側の点が除かれている） | CSV | E-04, E-02 |
| RT-05 | 延長線（左右）を追加した断面 | CSV | E-03, E-04 |
| RT-06 | 先頭の点の距離が 0 でない（距離標の基準） | CSV | **E-04** |
| RT-07 | 左端・右端の点を無効にしたもの | CSV | E-05, E-04 |
| RT-08 | 名前がすべて数値で、KP が昇順のもの | CSV | E-06 |
| RT-09 | 空白を含む断面名、"100" と "100.0" が混在する名前 | CSV | E-07 |
| RT-10 | 蛇行していて、断面線が中心線と 2 回以上交わるもの | CSV | E-08 |
| RT-11 | 座標が大きい（UTM 相当の 7 桁）＋プロジェクトのオフセットあり | CSV | E-09, E-10 |
| RT-12 | オフセットが異なるプロジェクトへ取り込む | CSV | E-10 |

（旧 RT-02「中点と CSV の比較」は、R-6 の回答で中点の設定が対象外になったため削除した。）

**RT-13 の具体的な手順**（`check.ipro` を一時フォルダにコピーして使う）:

1. `check.ipro` を開き、2 件の測量データそれぞれについて、各断面の値（上の比較する値の 1〜7）を記録する。
2. 測量データ A を *.riv でエクスポートする。`A.riv` と `A.riv.csv` ができる。
3. 同じプロジェクトに `A.riv` をインポートする。中心点の設定は「CSV」とし、ファイルには `A.riv.csv` を指定する。
4. 元の A と再インポートした A' を断面ごとに比べる。**断面表示画面での横座標（値 6）の差**を断面ごとに出力する。
5. その差が、E-04 の予測式 `pos_first_active + leftShift_old` と一致するかを確かめる。一致すれば E-04 で確定。一致しない断面は、E-03・E-08 などを個別に調べる。
6. 測量データ B でも同じことを行う。
7. 断面表示画面を開き、A と A' を重ねて表示したスナップショットを保存する（症状が見て分かる証跡）。

RT-13 はテストの基盤（§6）ができる前にも実施できる。まずは手作業か、使い捨てのテストコードで先に再現し、原因が E-04 かを早めに確かめる。
| RT-14 | 修正後の *.riv を**旧バージョンの iRIC**（修正前のビルド）で読み込む | CSV | 修正の互換性（D-8 の案で、他のツールや旧版でも読めること） |

加えて、層 A で次の 2 件をテストする（処理を純粋な関数に切り出した場合）。

- A-11: `findCrossSection` が、名前の一致する中心点の近くにある交点を選ぶこと（E-08）
- A-12: `.riv` の 1 行の分解処理が、空白を含む名前と数値の境界を正しく扱うこと（E-07）

※ 描画結果の画素単位の比較は行わない（フォントや DPI の環境差が大きいため）。描画の確認は §8 の手動テストで行う。

---

## 8. Phase 4〜5: 修正と手動テスト

### 8.1 修正のルール

- バグ 1 件ごとに: テストを追加して失敗を確認 → 最小限の修正 → テストが通ることを確認 → 関連テストを全件実行 → コミット。
- 修正は既存のコードスタイルに合わせる（`iric.astylerc`）。

### 8.2 自動化できない項目（人間による手動テスト）

自動テストでは判定が難しい次の項目を、チェックリストにして依頼する（§9 の R-4）。

| ID | 確認内容 |
|---|---|
| M-01 | 断面図の描画: 断面線・ODN マーカー・ラベルの重なり回避・中央の破線・水位線・植生の矩形・参照断面の半透明表示が正しく見えるか |
| M-02 | マウス操作の使い勝手: ドラッグの追従、カーソル形状、ラバーバンド選択、ホイールズーム、右クリックメニュー |
| M-03 | ステータスバーの座標表示（左岸からの距離と中心からの距離の切り替え） |
| M-04 | 実データでの動作: 国交省形式・北海道開発局 Web 取り込み（#1549）・実プロジェクトでの各操作 |
| M-05 | 日本語 UI での文言・レイアウトの崩れ、高 DPI 環境 |
| M-06 | 複数の断面ウィンドウを同時に開いたときの同期（片方での編集がもう片方に反映されるか） |
| M-07 | 修正前後での見た目の差（回帰の目視確認） |
| M-08 | `check.ipro` でエクスポート → インポート（中心点の設定は CSV）を行い、断面表示画面で元の測量データと重ねて表示し、断面図が横にずれていないことを目で確認する。平面図でも中心線・断面線の位置を確認する。修正前に 1 回（症状の確認）、修正後に 1 回（解消の確認） |

---

## 9. 人間への依頼事項

### いま依頼したいこと（Phase 1〜2 の着手前）

| ID | 依頼内容 | 理由 | 回答（2026-10-20） |
|---|---|---|---|
| R-1 | 河川測量データを含む **テスト用プロジェクト (.ipro) または .riv ファイル** の提供。可能なら、測量データ 2 件以上・ODN 設定あり・植生 (jmk) あり・水位データあり・断面名に数値と非数値が混在、の条件を満たすもの | 合成データだけでは実データ特有の問題を見逃すため。社外秘でないデータを希望 | **回答済み**: `E:\projects\05_iRIC_土木研など\2024\06_開発公営 GUI機能追加\10_作業履歴\20251009_受領データに基づくチェック\check.ipro`（横断測量データ 2 件を読み込み済み。ファイルの存在を確認済み）。扱いは §6.5 を参照 |
| R-2 | 統合テストで使う **ソルバー定義** の指定。例: `iRICsolvers_v4_Nays2DH`。開発環境で `iRICRootPath`（`apps/iricgui/debug`）から見える配置になっているか | 統合テストでプロジェクトを新規作成するため | **回答済み**: `iRICsolvers_v4_Nays2DH` を使う（`apps/solvers/iRICsolvers_v4_Nays2DH/definition.xml` があることを確認済み） |
| R-3 | 依存 DLL（VTK・GDAL・Qwt など）の bin ディレクトリの場所。普段 Debug 版の iRIC を起動するときに設定している PATH | テストの exe を単体で起動するときの PATH 設定に使うため | **回答済み（訂正）**: `start_cmd.bat` ではなく **`E:\programs\iRIC\start_vc.bat`**。各ライブラリの DLL の場所が書かれていることを確認した。最後の行で `devenv.exe` を起動するため、テストでは `devenv` の行を除いて使う（§6.5） |
| R-6 | 「エクスポート → インポートでずれる」報告の詳細を、報告者に確認していただきたい。(1) 使った形式（*.riv、*.odn、HONRYU の txt/csv、国交省 CSV など）、(2) インポート時の中心点の設定（中点・最低河床・CSV）と並び順、(3) 何がずれたか（平面図の中心線か、データ全体の平行移動か、断面の向きか、断面図の横位置か）とその量、(4) 元データに延長線・無効な点・低水路の 4 点があるか、(5) iRIC のバージョン、(6) 可能ならデータ（元のプロジェクトと、エクスポートしたファイル一式） | 候補 E-01〜E-11 のどれに当たるかを絞り込み、RT-13 で報告を直接再現するため | **回答済み**: 形式は *.riv。ずれたのは「断面表示画面で、断面図が横に移動していた」。ファイル一式はないので、`check.ipro` から *.riv をエクスポートし、*.riv.csv と一緒にインポートして試す（RT-13）。中心点を CSV で決める設定でずれるかを確認する。CSV を使わない設定でずれるのは仕様 |

### Phase 5 で依頼すること

| ID | 依頼内容 |
|---|---|
| R-4 | §8.2 のチェックリスト（M-01〜M-08）の実施と結果の記入。修正前後の 2 回を想定。結果の書式（OK / NG / 気付き、スクリーンショット）はテンプレートを用意する |
| R-5 | 統合テスト (層 B) は対話型のデスクトップセッションが必要。定期的に回す場合の実行マシンと実行タイミング（手動か、タスクスケジューラか）の決定 |

---

## 10. 判断が必要な事項

| ID | 内容 | 推奨 |
|---|---|---|
| D-1 | §6.3 のテストしやすくするための本体改修（ロジックの切り出し、objectName の付与）を行ってよいか | 行う（挙動は変えない） |
| D-2 | cppcheck を開発マシンにインストールしてよいか | インストールする |
| D-3 | B-03（マウス編集で左側の点が 1 つ多く消える）は不具合として修正してよいか。ダイアログ版と同じく、`position >= クリック位置` の点だけを消す仕様に揃える | 修正する |
| D-4 | B-05（未設定の ODN が編集のたびに設定済みになる）は不具合か、意図した仕様か | 未設定のまま保つよう修正する |
| D-5 | B-06（ウィンドウを閉じた後の Undo）の修正方針: (a) ウィンドウを閉じるときに Undo スタックをクリアする（既存の他機能と同じ方式）、(b) コマンドが `QPointer` を持ち、ウィンドウが消えていれば UI の更新を省く | (b)（ユーザーの Undo 履歴を失わない） |
| D-6 | テスト関連ファイルの追加先（`tests/` 配下、`tools/run_riversurvey_tests.ps1`）と、`tests.sln` / `tests.pro` への登録 | 案のとおり |
| D-7 | *.riv の往復で何を保存すべきか。*.riv 形式には有効フラグ・延長線・`leftShift` を書く欄がない。(a) 形式は変えずに、保存できる範囲（中心点、距離の基準）だけを保存する。保存できない情報（無効な点、延長線）は、エクスポート時に警告して知らせる。(b) 拡張した欄を追加して、すべて保存する（他のソフトとの互換性に影響がある） | (a) |
| D-8 | E-04 の修正案（エクスポート側だけを直す）: 左岸の座標として「左岸からの距離 0 の基準点」（`crosssectionPosition(-leftShift)`）を書き出し、各点の距離を `position + leftShift`（＝左岸からの距離）で書き出す。CSV を使ってインポートすると、中心点までの距離が元の `leftShift` と同じになるため、断面図が横に動かなくなる。*.riv 形式は変えないので、旧版や他のツールでもそのまま読める。注意点が 2 つある。(1) 延長線を左に付けた断面では、距離が負の点ができる。インポートでそのまま読めることを RT-05 で確かめる。(2) 中心点が基準点より左にある（`leftShift < 0`）データでは、`findCrossSection` が距離を絶対値で返すため、符号が逆になる。RT-06 の変形で確かめ、必要ならインポート側も直す | この案で進める |
| D-9 | E-06（名前がすべて数値のときの並び順）: 並び替えをやめてファイルの順序を保つか、現状の KP 降順を仕様として残すか。R-6 により、今回の「横にずれる」報告とは関係がないと判断できる | 現状の仕様を残す。`doInitWithSetting` で並べ替えをしない不整合だけを、別の修正候補として扱う |

---

## 11. 成果物

1. 更新した本ファイル（調査結果、確定したバグの一覧、修正内容）
2. `tests/riversurvey_logic/`、`tests/riversurvey_crosssectionwindow/`（自動回帰テスト）
3. `tools/riversurvey_tests/run_riversurvey_tests.ps1`（1 コマンドでビルド・実行・集計まで行う）
4. バグ修正コミット（1 バグ 1 コミット。実施では分野ごとにまとめた。§12.5）
5. 手動テストのチェックリスト（M-01〜M-08）と、その実施結果
6. エクスポート → インポートでずれる問題について: 原因の特定結果（E-xx のうち確定したもの）、往復テスト（RT-01, RT-03〜RT-14）、修正コミット、*.riv の往復で保存される情報・されない情報の整理（必要ならマニュアルへの追記案も）

---

## 12. 実施結果（2026-10-20）

作業ブランチ: `fix/riversurvey-crosssection`

### 12.1 静的解析

| ツール | 結果 |
|---|---|
| MSVC `/analyze`（riversurvey ライブラリ全体） | 586 件。大半は C26812（enum class 推奨）と C26451（算術オーバーフロー）の指摘。実害のあるものは次の 4 件: `MouseShiftRiverPathCenterCommand` の nullptr 参照外し（C6011）、`VegetationDataTableDelegate::createEditor` と `GeoDataRiverSurveyJmkExporter::doExport` の戻り値なし（C4715）、`.riv` 読み込みの未初期化変数 `mode`（C6001） |
| cppcheck 2.22（GitHub の公式版をユーザーフォルダ `%LOCALAPPDATA%\cppcheck` に展開して使用。システムへのインストールはしていない） | 1,316 件。大半はスタイルの指摘。実害のあるものは MSVC と同じ 4 件 |

### 12.2 確定したバグと修正

「再現」は、修正前のコードでテストが失敗（または異常終了）したことを確認したものです。

| ID | 内容 | 再現 | 修正 |
|---|---|---|---|
| B-01 | 点を降順に選んで削除すると、違う点が消える／範囲外アクセスで異常終了 | ✅ | `removePoint` で index を並べ替える |
| B-02 | 植生を降順に選んで削除すると異常終了 | ✅（異常終了） | index を並べ替える |
| B-03 | マウスで左側を編集すると、残すべき点が 1 つ余分に消える | 単体テストで確認 | `GeoDataRiverCrosssectionLogic::editFromPoint` に切り出して修正 |
| B-04 | ODN の付け直しで `end()` の参照外し | 単体テストで確認 | `remapOdn` に切り出して修正 |
| B-05 | 未設定の ODN が、編集のたびに設定済みになる | ✅ | 未設定のまま保つ（D-4） |
| B-06 | ウィンドウを閉じた後に Undo すると異常終了 | ✅（アクセス違反） | Undo コマンドが持つウィンドウを `QPointer` にする（D-5 (b)） |
| B-07, B-08 | 編集対象がない状態で各操作をすると異常終了 | ✅（アクセス違反） | 各操作の入口で nullptr を確認し、アクションも無効にする |
| B-09 | "100" と "100.0" がコンボボックスに重複して出る | 単体テストで確認 | 数値として同じ名前は 1 つにまとめる |
| B-10 | 水位表の表示と編集で、断面名のキーが違う | 未再現（該当データなし） | 対象の測量データ自身の断面名をキーに統一する |
| B-11 | 参照断面のコンボボックスが空のとき `at(-1)` | 修正前の DLL ではテストの前提を満たせず未確認 | 範囲を確認する |
| B-12 | 点を削除しても固定点の index がずれたまま（以降の参照で例外） | ✅（例外） | 削除した数だけ詰め、失敗時は元に戻す |
| B-13〜B-16 | 1 点だけの断面、コンボボックスの -1、親のない QAction、空選択での `at(0)` | — | ガードを追加 |
| B-17（新規） | ODN の点の位置を表で編集すると、ODN が端の点に付け替わる | ✅ | 点の数が変わらない編集では index を保つ |
| G-1（新規） | 編集対象がない状態でマウスを動かすと異常終了 | ✅（B-07 のテストで一緒に確認） | ガードを追加 |
| G-5（新規） | 格子生成モードのドラッグ終了時に、別の測量データの格子生成条件を使う | — | ドラッグ中と同じものを使う |
| G-6（新規） | 植生追加モードで右クリックすると nullptr 参照外し、植生ダイアログも出る | — | 左ボタンだけを処理する |
| G-7（新規） | 右クリックメニューの「面積計算」が、最初に対象だった測量データのアクションに固定される | — | ウィンドウのアクションを使う（未実装だった `calcAreaAction()` を実装） |
| G-8（新規） | 断面より左にはみ出した植生を描くと例外で異常終了 | ✅（例外） | 描画をスキップする |
| G-9（新規） | `mousePressEvent` の `case` に `break` がなく、植生ドラッグの開始時に点のクリックも発生する | — | `break` を追加 |
| S-01（新規） | 中洲掘削（`EditSlopePointCommand`）で ODN が未設定だと `at(-1)` で例外。マージ時に ODN が更新されない | — | `remapOdn` を使う。マージ時に ODN も更新する |
| V-01（新規） | 植生表の「冠水」のチェックを外せない（常に 1 が書かれる） | ✅ | チェック状態を書く |
| V-02（新規） | `VegetationDataTableDelegate::createEditor` と `GeoDataRiverSurveyJmkExporter::doExport` の戻り値なし | 静的解析 | 戻り値を追加 |
| V-03（新規） | `.riv` 読み込みで変数 `mode` が未初期化 | 静的解析 | 0 で初期化 |
| V-04（新規） | `MouseShiftRiverPathCenterCommand` で、選択がないと nullptr 参照外し | 静的解析 | ガードを追加 |

### 12.3 エクスポート → インポートでずれる問題（E-04）

- `check.ipro` の 2 件（H30、H25。各 169 断面）で再現した。中心点と平面座標は元どおりだが、**断面表示画面の横位置が、全断面で予測式 `pos_first_active + leftShift_old` とちょうど同じ量だけずれた**（例: 22.1 m、30 m）。これで原因を E-04 に確定した。
- 修正（D-8）: エクスポート時に、左岸の座標として「左岸からの距離 0 の点」を書き、各点の距離もそこから測った値を書く。中心点が基準点より左にある（`leftShift < 0`）場合だけ、従来どおり有効な左岸点を基準にする。*.riv の形式は変えていない。
- *.riv で保存できない情報（無効な点、延長線）があれば、エクスポート時に警告を出す（D-7 (a)）。
- 修正後、338 断面のすべてで、中心点・断面表示画面の横位置・平面座標・標高が元どおりになった（許容誤差は 1 mm。点ごとの比較は 1 cm。インポート時に同じ距離の点が 1 mm ずらされる仕様があるため）。

### 12.4 自動回帰テスト

| スイート | 場所 | 件数 | 修正前 | 修正後 |
|---|---|---|---|---|
| 単体テスト | `tests/riversurvey_logic/` | 26 | 3 件失敗（B-01, B-12） | 26 件成功 |
| GUI 統合テスト | `tests/riversurvey_crosssectionwindow/` | 12 | 11 件失敗（うち 5 件は異常終了） | 12 件成功 |

実行方法（Visual Studio 2019 と Qt 5.14.2 の環境は `start_vc.bat` から自動で読み込む）:

```powershell
# ビルドして全テストを実行する
powershell -ExecutionPolicy Bypass -File tools\riversurvey_tests\run_riversurvey_tests.ps1 -Build
# 単体テストだけ（数秒）
powershell -ExecutionPolicy Bypass -File tools\riversurvey_tests\run_riversurvey_tests.ps1 -Suite logic
# 特定のテストだけ
powershell -ExecutionPolicy Bypass -File tools\riversurvey_tests\run_riversurvey_tests.ps1 -Suite gui -Functions exportImportRoundTripWithCsvCenterLine
```

- 結果は `tests/riversurvey_results/`（git の管理外）に出力される。テストごとのログと xUnit XML、`summary.csv`、往復テストの断面ごとの報告 `roundtrip_report.txt`。
- GUI テストは iRIC 本体をテストの中で起動するので、デスクトップにウィンドウが表示される。1 件あたり約 30 秒かかる。
- テストは iRIC の設定を、アプリケーション名「iRIC GUI 4 RiverSurveyTest」の領域に保存する。通常の iRIC の設定（作業フォルダ、「次回から表示しない」など）には影響しない。
- iRIC のルートフォルダは `apps\iricgui\<構成>` を使う（ソルバーは `apps\solvers` から読み込まれる）。

### 12.5 プランからの変更点

- 実行スクリプトの場所は、計画の `tools/run_riversurvey_tests.ps1` ではなく `tools/riversurvey_tests/run_riversurvey_tests.ps1` にした（補助スクリプトと同じフォルダにまとめた）。
- 「1 バグ 1 コミット」の予定だったが、同じファイルへの修正が多く重なっていたため、コミットは分野ごとに分けた。各バグとテストの対応は §12.2 のとおり。
- 合成データによる往復テスト（RT-01, RT-03〜RT-12, RT-14）は、実データ（RT-13）で原因が確定し修正も確認できたため、まだ作っていない（§12.6）。

### 12.6 未対応・残作業

| 項目 | 内容 |
|---|---|
| 手動テスト（R-4） | §8.2 の M-01〜M-08 を、修正後のビルドで実施していただきたい。特に M-08（`check.ipro` の往復を目で確認）と、描画まわり（M-01） |
| E-03, E-05 | 延長線と無効な点は *.riv に保存できない。エクスポート時の警告で知らせるだけにした（D-7 (a)）。警告文の翻訳（.ts）はまだ更新していない |
| E-06, E-07, E-08 | 並び順、空白を含む断面名、蛇行部での中心点の判定は、今回のずれとは関係がないので修正していない（D-9） |
| 合成データの往復テスト | RT-01, RT-03〜RT-12, RT-14 は未作成 |
| B-10 のテスト | 断面名の表記が違うデータ（"100" と "100.0"）を含むテストデータがないため、GUI テストはまだない |
| Release ビルド | テストは Debug ビルドだけで実施した（範囲外アクセスを確実に検出するため） |
