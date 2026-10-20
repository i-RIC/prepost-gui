# 横断測量データのインポート・エクスポートに関する調査報告

- 対象ブランチ: `fix/riversurvey-crosssection`（分岐元 `a87ed22b0`）
- 対象コミット: `45126698f`（*.riv エクスポートの修正）、`a752d7f98`（*.riv 読み込みの未初期化変数など）、`cff5e5ce3` / `8040ef4f4`（ODN の点番号の付け替えなど）
- 調査方法: ソースコードを読んで確認した（静的な調査）。今回、新たにビルドやテストは実行していない。*.riv の往復については、ブランチの GUI テスト `exportImportRoundTripWithCsvCenterLine` が `check.ipro` の 338 断面で確認済み（[river_survey.md](../plans/river_survey.md) §12.3）。

---

## 1. *.riv エクスポートの修正内容

### 1.1 結論

- ご理解のとおり、**修正前は「有効な点のうち一番左の点」を距離 0 として書き出していた**。そのため、書き出したファイルでは先頭の点の距離が必ず 0 になっていた。
- ご指摘のとおり、*.riv の形式を変えずに正しい「左岸からの距離」を書き出すには、内部にそのための情報が必要になる。ただし、**その情報は以前からあった**。`GeoDataRiverCrosssection::m_leftShift`（以下 `leftShift`）がそれで、build 1920 より後のプロジェクトには保存されている。今回の修正では、内部データを新たに追加していない。
- 修正後は、`leftShift` から「左岸からの距離が 0 になる点」（以下「距離の基準点」）を計算し、次のように書き出す。
  - `#survey` の左岸座標: 距離の基準点の座標
  - `#x-section` の各点の距離: 距離の基準点からの距離（＝断面表示画面に表示している「左岸からの距離」）
- *.riv 形式では、`#survey` の左岸座標が先頭の点と一致している必要はなく、先頭の点の距離が 0 でなくてもよい。このため、形式を変えずにこの情報を書き出せる。
- *.riv から取り込んだデータでは、距離の基準点は元のファイルの `#survey` の左岸座標（距離標）と同じ点になる。つまり、修正後のエクスポートでは、元のファイルの左岸座標と距離がそのまま書き出される。

### 1.2 内部データ: `position` と `leftShift`

各断面（`GeoDataRiverPathPoint`）は、次の値を持っている。

| 値 | 意味 |
|---|---|
| `m_position` | 中心点（河川中心線上の点）の平面座標 |
| `m_crosssectionDirection` | 断面の方向（左岸から右岸への単位ベクトル） |
| `Altitude::position()` | 各点の「**中心点からの距離**」（左が負、右が正） |
| `leftShift()` | 「中心点からの距離」を「左岸からの距離」に変えるための値。**左岸からの距離 = `position() + leftShift()`** |

断面表示画面の横軸は、すべて `alt.position() + leftShift` で描いている（[geodatariversurveycrosssectionwindowgraphicsview.cpp](../libs/geodata/riversurvey/geodatariversurveycrosssectionwindowgraphicsview.cpp)）。表の「左岸からの距離」の列も同じ値である（[geodatariversurveycrosssectionwindow.cpp](../libs/geodata/riversurvey/geodatariversurveycrosssectionwindow.cpp)）。

`leftShift` は次の処理で更新される。プロジェクトへの保存と読み込みは [geodatariverpathpoint.cpp](../libs/geodata/riversurvey/geodatariverpathpoint.cpp) の `save()` / `load()` で行う。読み込みは build 1920 より後のファイルだけが対象である。

| 処理 | 実装 | `position` の変化 | `leftShift` の変化 |
|---|---|---|---|
| 中心点の移動（`shiftCenter(w)` → `moveCenter(w)`） | [geodatarivercrosssection.cpp](../libs/geodata/riversurvey/geodatarivercrosssection.cpp) `moveCenter` | `-= w` | `+= w` |
| 断面の拡幅（`expand(r)`） | 同 `expand` | `*= r` | `*= r` |
| インポート（*.riv、MLIT、ODN） | 各インポータで `shiftCenter(shiftValue)` を呼ぶ | 下記 | 下記 |

**インポート時**（[geodatariversurveyimporter.cpp](../libs/geodata/riversurvey/geodatariversurveyimporter.cpp) の `importData`）:

1. 中心点を、ファイルの左岸座標 `L` に置く。断面の方向は `normalize(R - L)` にする。
2. 各点の `position` には、ファイルに書かれた距離 `d_i` をそのまま入れる。この時点では `leftShift = 0`。
3. 中心点の設定（中点、最低河床、CSV）に従って `shiftValue = s` を決め、`shiftCenter(s)` を呼ぶ。
   その結果、`position_i = d_i - s`、`leftShift = s`、中心点 `C = L + dir * s` になる。

したがって、インポート直後は次の関係が成り立つ。

- 左岸からの距離 `position_i + leftShift = d_i`（ファイルに書かれた距離そのもの）
- `C - dir * leftShift = L`（**ファイルの左岸座標**）

中心点を移動しても、`C` が `dir * w` 動き、`leftShift` が `w` 増えるので、`C - dir * leftShift` は変わらない。つまり「距離の基準点 `crosssectionPosition(-leftShift)`」は、断面を回転・拡幅しない限り、**元のファイルの左岸座標（距離標）の位置に固定されている**。今回の修正は、この性質を使っている。

### 1.3 修正前の挙動

[geodatariversurveyexporter.cpp](../libs/geodata/riversurvey/geodatariversurveyexporter.cpp)（修正前）:

```cpp
QPointF leftBank  = tmpp->crosssectionPosition(tmpp->crosssection().leftBank(true).position());
...
double left = tmpp->crosssection().leftBank(true).position();
outstream << alt.position() - left << "\t" << alt.height() << "\t";
```

`leftBank(true)` は「有効な点のうち一番左の点」である。そのため、次のようになっていた。

- `#survey` の左岸座標は、一番左の有効な点の座標
- 距離は、一番左の有効な点からの距離。**先頭の点は必ず 0**

再インポートすると `leftShift` が「一番左の点から中心点まで」で再計算される。そのため、断面表示画面の横位置が全体で次の量だけずれていた。

> ずれ量 = 元の一番左の有効な点の左岸からの距離 = `position_first + leftShift_old`

元データの先頭の点が距離標から離れている場合（国交省形式のデータで多い）、4 点ありで取り込んで外側の点が除かれた場合、左端の点を無効にした場合、左に延長線を追加した場合に、このずれが起きる。

### 1.4 修正後の挙動

```cpp
double distanceOrigin(GeoDataRiverCrosssection& cs)
{
	double origin = - cs.leftShift();
	if (origin > 0) {return cs.leftBank(true).position();}
	return origin;
}
...
QPointF leftBank  = tmpp->crosssectionPosition(distanceOrigin(cs));   // #survey の左岸座標
...
double left = distanceOrigin(tmpp->crosssection());
outstream << alt.position() - left << ...;                             // = position + leftShift
```

- 距離の基準は、`position = -leftShift`、つまり左岸からの距離が 0 になる点である。ここから測った距離 `position - (-leftShift)` は、断面表示画面の「左岸からの距離」と同じ値になる。
- `origin > 0`（`leftShift < 0`。距離の基準点が中心点より右にある）の場合は、従来どおり一番左の有効な点を基準にする。インポータは、CSV の中心線と「左岸座標から右岸座標までの線分」との交点を探す（`findCrossSection`）。基準点が中心点より右にあると、この線分が中心線と交わらない。そのため、この場合は従来の方法に戻している。
- 右岸座標は、従来どおり一番右の有効な点の座標である。

### 1.5 数値例

元の *.riv: 左岸座標 `L = (0, 0)`、右岸座標 `R = (100, 0)`、距離 `10, 20, 50, 80, 90`。中心線の CSV は x = 50 で交わる。

| | `#survey` 左岸座標 | 書き出す距離 | 再インポート後の `leftShift` | 断面表示画面の横軸 |
|---|---|---|---|---|
| インポート直後 | — | — | 50 | 10, 20, 50, 80, 90 |
| 修正前のエクスポート | (10, 0) | 0, 10, 40, 70, 80 | 40 | 0, 10, 40, 70, 80（**10 m ずれる**） |
| 修正後のエクスポート | (0, 0) | 10, 20, 50, 80, 90 | 50 | 10, 20, 50, 80, 90（元どおり） |

修正後のエクスポートでは、元のファイルの `#survey` と `#x-section` がそのまま書き出される。

### 1.6 再インポート時に何が元どおりになるか

インポート後の左岸からの距離は、常に `position_i + leftShift = d_i` になる（`shiftCenter` は値を範囲内に収めることがあるが、和は変わらない）。各点の平面座標も `L + dir * d_i` で、中心点の設定には関係しない。このため、修正後は次のようになる。

| 中心点の設定 | 左岸からの距離（断面表示画面の横位置） | 各点の平面座標 | 中心点の位置 |
|---|---|---|---|
| CSV（エクスポート時に一緒に書き出す `*.riv.csv`） | 元どおり | 元どおり | 元どおり |
| 左右岸の中点、最低河床 | **元どおり** | 元どおり | 設定に従って変わる（仕様） |

コミットメッセージでは「CSV を使うと元どおりになる」と書いているが、左岸からの距離は、中心点の設定に関係なく元どおりになる。CSV が必要なのは、中心点の位置を戻すためである。

### 1.7 制約と注意点

1. **`leftShift < 0` の断面は、従来どおりずれる。** 中心点を距離標より左に移動した場合などに起きる。改善案は §3 の D-2 を参照。
2. **`leftShift = 0` の断面**（iRIC で新規作成・追加した断面、build 1920 以前の古いプロジェクトのデータ）は、距離の基準点が中心点そのものになる。その結果、次のようになる。
   - `#survey` の左岸座標に、中心点の座標が書かれる。
   - 中心点より左の点の距離が**負の値**になる。断面表示画面の表示と同じ値ではあるが、修正前（先頭は 0）とは出力が変わる。負の距離を受け付けない外部ツールに渡す場合は注意が必要。
   - 仕様として受け入れるかどうか、確認をお願いしたい。
3. *.riv に保存できない情報（無効な点、延長線）がある場合は、エクスポート時に警告を出す。警告文の翻訳（.ts）はまだ更新していない。
4. 延長線がある断面では、`crosssectionPosition` が折れ線上の座標を返す。*.riv では直線として読み戻すため、断面の向きと中心点が変わることがある（従来からの制約。警告で知らせる）。
5. **`leftShift` が正しく保たれていることが前提になった。** 修正前の *.riv エクスポートは `leftShift` を使っていなかったが、修正後は使う。そのため、`leftShift` が壊れる不具合（§3 の A-1）が、*.riv の出力にも影響するようになった。

---

## 2. ほかの形式への影響

### 2.1 一覧

登録されているインポータとエクスポータ（[geodatariversurveycreator.cpp](../libs/geodata/riversurvey/geodatariversurveycreator.cpp)）について、今回のブランチによる変化を確認した。

| 形式 | クラス | 今回のコード変更 | 挙動の変化 |
|---|---|---|---|
| *.riv インポート | `GeoDataRiverSurveyImporter` | `mode` の初期化だけ | なし（不正なファイルで未定義動作になる可能性がなくなった） |
| *.riv エクスポート | `GeoDataRiverSurveyExporter` | §1 の修正 | あり（§1） |
| *.odn インポート | `GeoDataRiverSurveyOdnImporter` | なし | なし |
| *.odn エクスポート | `GeoDataRiverSurveyOdnExporter` | なし | **間接的に改善**（下記） |
| LandXML エクスポート | `GeoDataRiverSurveyLandXmlExporter` | なし | なし。ただし *.riv と距離の基準が異なるようになった（§3 の B-3） |
| CSV / テキスト エクスポート（HONRYU） | `GeoDataRiverSurveyHonryuExporter` | なし | なし。LandXML と同じく、距離の基準が *.riv と異なる |
| 国交省 CSV インポート | `GeoDataRiverSurveyMlitImporter` | なし | なし（`GeoDataRiverSurveyImporter::importData` を共有しているが、そこは変更していない） |
| Web インポート（北海道開発局） | `GeoDataRiverSurveyWebImporter` | なし | なし（国交省 CSV インポータに処理を任せている） |
| JMK エクスポート（エクスポートメニュー外） | `GeoDataRiverSurveyJmkExporter` | 戻り値を追加 | 戻り値が不定だったため、成否の判定が正しくなった |

### 2.2 ODN エクスポートへの間接的な影響

- ODN エクスポートは、以前から距離を `position + leftShift` で書き出している。今回の *.riv の修正で、*.riv と ODN の距離の基準がそろった。
- 断面を編集したときの ODN の点番号（Nb）の付け替えが、`GeoDataRiverCrosssectionLogic::remapOdn` にまとめられた（`cff5e5ce3`、`8040ef4f4`）。次の誤りが直ったので、ODN に書き出す Nb が正しくなる。
  - 表で点の位置を編集すると、Nb が端の点に付け替わる（B-17）。
  - 中洲掘削で Nb が更新されない、または例外が起きる（S-01）。

---

## 3. 見つかった問題と修正案

今回の修正と関係のないものも含む。重大度は「高 / 中 / 低」の 3 段階で示す。

### 3.1 一覧

| ID | 重大度 | 対象 | 内容 | 今回の修正との関係 |
|---|---|---|---|---|
| A-1 | **高** | 中心点のシフト、断面の拡幅 | 元に戻す（Undo）と、ダイアログの「適用」→「OK」/「キャンセル」で、`leftShift` が元に戻らない | **あり**（*.riv の出力に影響するようになった） |
| A-2 | 低 | 断面の挿入（断面をコピー） | `leftShift` がコピーされない | あり（コピーした断面は `leftShift = 0` の扱いになる） |
| B-1 | **高** | LandXML | 座標にオフセットを加えていない | なし |
| B-2 | 中 | LandXML | 縦断（`ProfAlign`）の標高が 1 断面ずれる | なし |
| B-3 | 中 | LandXML、HONRYU | 距離の基準が一番左の有効な点で、`leftShift` を使っていない | あり（*.riv と出力が異なるようになった） |
| C-1 | 中 | ODN エクスポート | 無効な点も警告なしに書き出す。再インポートすると有効な点になる | なし |
| C-2 | 中 | ODN エクスポート | 固定幅 `%8.2f` で桁あふれすると、値がつながって読めなくなる。精度は 1 cm | なし |
| C-3 | 低 | ODN 往復 | 未設定の Nb（-1）を 0 として書き出し、再インポートすると端の点になる | なし |
| C-4 | 低 | ODN インポート | 断面名が数値でないと、全断面が同じ位置になる。エラー時のメモリリーク。並べ替えると Nb がずれる | なし |
| D-1 | 中 | *.riv インポート | 列数の不足や不正な 4 点の番号で異常終了する。読み込みを黙って打ち切ることがある | なし |
| D-2 | 中 | *.riv インポート（CSV の中心線） | 最初に見つかった交点を使い、CSV の名前の列を使っていない。線分の外の交点を扱えない | あり（§1.7 の 1 の改善につながる） |
| D-3 | 低 | *.riv インポート | 「逆順」の設定が保存されない。設定を使った再読み込みで並べ替えない。"100" と "100.0" が同じ断面になる | なし |
| D-4 | 低 | *.riv インポート（CSV の中心線） | 空行があるとエラーになる | なし |
| E-1 | 低 | 国交省 CSV インポート | 設定を使った読み込み（`doInitWithSetting`）でファイルを読まない。そのほか細かい問題 | なし |
| E-2 | 低 | Web インポート | 通信エラーでも処理を続ける。`importInit` の戻り値を見ていない | なし |
| F-1 | 低 | *.riv、ODN、LandXML エクスポート | 断面が 1 つもないと nullptr を参照する | なし |

おすすめの対応順は、**A-1 → B-1 → B-3 / B-2 → D-1 → C-1 / C-2 → D-2** である。A-1 は今回の修正の効果を損なうので、同じブランチで直すのがよい。

### 3.2 A-1【高】中心点のシフトと断面の拡幅で `leftShift` が元に戻らない

**場所**: [geodatariversurvey_shiftriverpathcentercommand.cpp](../libs/geodata/riversurvey/private/geodatariversurvey_shiftriverpathcentercommand.cpp)、[geodatariversurvey_expandcrosssectioncommand.cpp](../libs/geodata/riversurvey/private/geodatariversurvey_expandcrosssectioncommand.cpp)

コンストラクタで `shiftCenter()` / `expand()` を呼ぶので、`leftShift` が変わる。しかし、`redo()` / `undo()` が書き戻すのは `position` と `AltitudeInfo()` だけで、`leftShift` は書き戻さない。

```cpp
// ShiftRiverPathCenterCommand::undo()
m_points[i]->setPosition(m_oldPositions[i]);
m_points[i]->crosssection().AltitudeInfo() = m_oldCrosssections[i];   // leftShift は戻らない
```

ダイアログ（[geodatariverpathpointshiftdialog.cpp](../libs/geodata/riversurvey/geodatariverpathpointshiftdialog.cpp)、[geodatariverpathpointexpanddialog.cpp](../libs/geodata/riversurvey/geodatariverpathpointexpanddialog.cpp)）は、「適用」でコマンドを積み、「OK」/「キャンセル」/「適用」のやり直しでは `undo()` してから積み直す。そのため、次のことが起きる。

| 操作 | 結果 |
|---|---|
| シフト → Ctrl+Z | 中心点は戻るが、`leftShift` はシフト量だけずれたまま。断面表示画面の横位置がずれる |
| 「適用」→「OK」 | `leftShift` にシフト量が **2 回加わる** |
| 「適用」→「キャンセル」 | 何も変えていないはずなのに `leftShift` が変わる |
| 拡幅で同じ操作 | `leftShift` に倍率が 2 回かかる、またはかかったまま残る |

各点の平面座標は正しいままだが、「左岸からの距離」が狂う。影響は断面表示画面の表示、ODN エクスポート、そして**今回修正した *.riv エクスポート**に及ぶ。*.riv では、`#survey` の左岸座標と距離が同じ量だけずれる。ずれ量によっては `leftShift < 0` になり、従来の方法に戻る。

なお、マウスで中心点を動かすコマンド（`MouseShiftRiverPathCenterCommand`）は、`shiftCenter(±v)` を対称に呼ぶので問題ない。

**修正案**: 前後の `leftShift` も保存して書き戻す。

```cpp
// コンストラクタ
m_oldLeftShifts.append(p->crosssection().leftShift());
p->shiftCenter(offset);
m_newLeftShifts.append(p->crosssection().leftShift());
// redo()
m_points[i]->crosssection().setLeftShift(m_newLeftShifts[i]);
// undo()
m_points[i]->crosssection().setLeftShift(m_oldLeftShifts[i]);
```

`ExpandCrosssectionCommand` も同じように直す。テストとしては、「シフト → Undo」と「適用 → OK」のあとで `position + leftShift` が期待どおりかを確かめるものを `tests/riversurvey_crosssectionwindow` に追加する。

### 3.3 A-2【低】断面をコピーして挿入すると `leftShift` がコピーされない

**場所**: [geodatariverpathpointinsertdialog.cpp](../libs/geodata/riversurvey/geodatariverpathpointinsertdialog.cpp)（「断面をコピー」の処理）

`AltitudeInfo()` だけをコピーしているので、コピー先の断面は `leftShift = 0` になる。断面表示画面で、コピー元と横位置が `leftShift` の分だけ違って見える。エクスポートすると §1.7 の 2 の扱い（負の距離）になる。

**修正案**: `m_newPoint->crosssection().setLeftShift(p->crosssection().leftShift());` を追加する。

### 3.4 B-1【高】LandXML の座標にオフセットが加わっていない

**場所**: [geodatariversurveylandxmlexporter.cpp](../libs/geodata/riversurvey/geodatariversurveylandxmlexporter.cpp)

iRIC の内部座標は、プロジェクトのオフセットを引いた値である（`GeoDataRiverSurvey::doApplyOffset`）。*.riv エクスポートや、点群データの LandXML エクスポート（[geodatapointmaplandxmlexporter.cpp](../libs/geodata/pointmap/geodatapointmaplandxmlexporter.cpp)）は、オフセットを足して書き出している。一方、横断測量データの LandXML エクスポートは、次のどちらにもオフセットを足していない。

- 距離標の線形（`CoordGeom`）の座標
- 地表面（`Surfaces/Pnts`）の座標

そのため、オフセットを設定したプロジェクトでは、**座標がオフセットの分だけずれた LandXML が出力される**。

**修正案**: `pd->mainfile()->offset()` を取得し、`coordStr(p)` と `Pnts` の出力で足す。

### 3.5 B-2【中】LandXML の縦断の標高が 1 断面ずれる

**場所**: 同ファイルの `writeAlignmentsForLeftDistanceMarkAndCrossSection` の `Profile` 部分（132 行目と 139 行目付近）

```cpp
double elev = point->crosssection().leftBank(true).height();   // 1 番目: (0, 断面1)
while (point->nextPoint() != nullptr) {
	auto p = point->leftBank()->interpolate(1);                 // 次の断面までの距離
	len += iRIC::distance(prevP, p);
	double elev = point->crosssection().leftBank(true).height(); // ← 現在の断面の標高
```

ループの中で、距離は次の断面まで進めているのに、標高は現在の断面のものを使っている。そのため、PVI は `(0, 断面1), (s2, 断面1), (s3, 断面2), …` となり、標高が 1 断面ずつずれる。最後の断面の標高は出力されない。

**修正案**: ループの中では `point->nextPoint()->crosssection().leftBank(true).height()` を使う。

### 3.6 B-3【中】LandXML と HONRYU で、距離の基準が `leftShift` を使っていない

**場所**: LandXML の `CrossSect/PntList2D`（177 行目 `a.position() - lb.position()`）と距離標の線形（`leftBank()->interpolate(0/1)`）、[geodatariversurveyhonryuexporter.cpp](../libs/geodata/riversurvey/geodatariversurveyhonryuexporter.cpp)（`alt.position() - leftPos`）

どちらも修正前の *.riv と同じく、一番左の有効な点を距離 0 としている。そのため、断面表示画面、ODN、修正後の *.riv とは距離の値が異なる。特に LandXML（国交省の河川定期縦横断データの形式）では、左岸距離標の座標と、そこからの距離を書くことになっている。現状は、一番左の点を距離標として書き出している。

**修正案**:

- `distanceOrigin()` を [geodatariversurveyexporter.cpp](../libs/geodata/riversurvey/geodatariversurveyexporter.cpp) の無名名前空間から、`GeoDataRiverCrosssection` または共通のユーティリティに移し、3 つのエクスポータで共通に使う。
- LandXML では、`PntList2D` の距離を `alt.position() - distanceOrigin(cs)` にする。距離標の線形（`left_distance_mark`）の座標は、`crosssectionPosition(distanceOrigin(cs))` を断面ごとに結んだ折れ線にする（今は左岸線の補間の端点、つまり各断面の一番左の有効な点を結んでいる）。
- HONRYU は、受け取る側が「先頭 0」を前提にしている可能性がある。設定ダイアログに「距離の基準: 左岸距離標 / 一番左の点」の選択を加え、既定値をどちらにするか決めていただきたい。

### 3.7 C-1【中】ODN エクスポートで無効な点の扱いが不明

**場所**: [geodatariversurveyodnexporter.cpp](../libs/geodata/riversurvey/geodatariversurveyodnexporter.cpp)

ODN エクスポートは、無効な点も含めてすべての点を書き出す（Nb の番号をそろえるため）。ODN 形式には有効かどうかのフラグがないので、再インポートすると**無効な点が有効な点になる**。*.riv では無効な点を書き出さず、今回の修正で警告も出すようにしたが、ODN には警告がない。

**修正案**: *.riv と同じ警告を出す。または、無効な点を除いて書き出し、Nb を `remapOdn` で付け替える。後者の方が *.riv と挙動がそろう。

### 3.8 C-2【中】ODN の固定幅による桁あふれと精度

**場所**: 同ファイル（`QString("%1").arg(v, 8, 'f', 2)`）

- 距離と標高を幅 8、小数 2 桁で書いている。値が 8 文字以上（`-1000.00` 以下、`10000.00` 以上）になると、前の値と空白なしでつながる。インポータは空白で区切って読むので、読み込みエラーになる。
- 精度は 1 cm なので、往復で丸めが起きる。
- 断面名（幅 10）、Nb（幅 5）も、幅を超えると列がずれる。

**修正案**: 書き出す前に範囲を確かめ、幅に収まらない値があれば警告する（ODN は固定幅の古い形式なので、形式は変えない）。

### 3.9 C-3【低】未設定の ODN 点番号が往復で端の点になる

`remapOdn` は、未設定の Nb を -1 のまま残す。エクスポータは `nb + 1 = 0` を書き、インポータは `0 - 1 = -1` を範囲外と見なして端の点（0 または最後）にする。往復すると、未設定だった Nb が設定済みになる。`GeoDataRiverSurveyOdnChecker` は -1 を含む並び順を「順序が不正」と判定する可能性もある。

**修正案**: エクスポート前のチェック（`GeoDataRiverSurveyOdnChecker`）で、未設定の Nb を問題として報告する。

### 3.10 C-4【低】ODN インポートのそのほかの問題

**場所**: [geodatariversurveyodnimporter.cpp](../libs/geodata/riversurvey/geodatariversurveyodnimporter.cpp)

- 断面の位置を `y = -name.toFloat() * 1000` で決めている。断面名が数値でないと、全断面が `y = 0` に重なる。位置の設定ダイアログで「KP」を選ぶと、形状が壊れる。断面名が数値でない場合は、「距離」を選ぶように誘導するか、警告を出す。
- エラーで `return false` するとき、作成した `GeoDataRiverPathPoint` を解放していない。
- 点を `cs.addPoint(pos, e)` で並べ替えながら追加している。ファイルの点が距離順でないと、ファイル上の順番を前提にした Nb がずれる。同じ距離の点を少しずらす処理（*.riv の `shiftUniqueAlts`）もない。

### 3.11 D-1【中】*.riv インポートで異常終了するファイルがある

**場所**: [geodatariversurveyimporter.cpp](../libs/geodata/riversurvey/geodatariversurveyimporter.cpp) `readRivFile` と `importData`

- `#survey` の行の列が 5 未満のとき、`tokens.at(1)`〜`at(4)` が範囲外になる（125 行目付近）。Debug ビルドでは assert、Release ビルドでは未定義動作になる。
- 4 点の番号（`divIndices`）の範囲を確かめていない。0 や点数を超える値があると、`p->altitudes.at(p->divIndices[1] - 1)` で `std::out_of_range` が発生し、異常終了する（501 行目付近）。
- `#x-section` の断面名の行に点数がないと、`break` でファイルの残りを**黙って読み飛ばす**（155 行目）。
- `f.open()` が失敗しても確かめていない（99 行目）。

**修正案**: 列数と番号の範囲を確かめ、行番号付きのエラーメッセージを出す。既存の書き方（`QMessageBox::critical(... "%1 line %2 ...")`）に合わせる。

### 3.12 D-2【中】CSV の中心線との交点の求め方

**場所**: 同ファイル `findCrossSection`（319 行目）

- 中心線と最初に交わった区間を採用している。蛇行していて断面線が中心線と複数回交わると、別の交点を中心点にしてしまう（計画書の E-08）。iRIC が書き出す `*.riv.csv` には `name` の列があるのに、使っていない。
- 交点を「左岸座標から右岸座標までの線分の中」に限っている。このため、§1.7 の 1 の `leftShift < 0` の断面は、従来の方法に戻すしかなかった。

**修正案**:

1. CSV に 3 列目（名前）があり、断面名と一致する行があれば、その点を断面線に投影した位置を中心点とする。
2. 交点の判定を「直線上で、断面の点の範囲（最初の点〜最後の点）にある」に広げる。`shiftValue` は符号付きの距離とする。そうすれば、エクスポータの `origin > 0` の例外処理をなくし、常に `-leftShift` を基準に書き出せる。

### 3.13 D-3【低】並び順と断面名の扱い（計画書の E-06 / E-07）

- `doInit` で、ダイアログの「逆順」を `m_reverseOrder` に入れていない。そのため、設定に `reverseOrder` として保存される値が常に初期値になる（409 行目）。
- `doInitWithSetting` では、KP による並べ替えも逆順も行わない。
- 断面名をすべて数値と見なせると、`realKP` で同じ断面と判定する。そのため、"100" と "100.0" が 1 つの断面にまとめられる。空白を含む断面名は、空白で区切るため正しく読めない（形式上の制約）。

### 3.14 D-4【低】中心線の CSV で空行がエラーになる

`loadPoints` は、列が 2 未満の行をエラーにする。そのため、途中や末尾に空行（`,` を含まない行）があると、読み込みが失敗する。1 行目を見出しとして読み飛ばすときに `lineNo` を増やしていないため、2 行目以降の行番号の表示も 1 つずれる。

**修正案**: 空行は読み飛ばし、見出しを読み飛ばすときも `lineNo` を増やす。

### 3.15 E-1【低】国交省 CSV インポート

**場所**: [geodatariversurveymlitimporter.cpp](../libs/geodata/riversurvey/geodatariversurveymlitimporter.cpp)

- `doInitWithSetting` がファイルを読まない（`m_points` が空のまま）。設定を使って読み込み直すと、空のデータになる。断面フォルダも設定に保存していない。
- `tr("Distance marker data is not found. Will be ignored.").arg(dmFile)` に `%1` がない（Qt が実行時に警告を出す）。
- `uniquedDistances` は、どこからも値が入らない（使われていないコード）。
- 低水路の 4 点の番号（フラグ 13）を、並べ替える前の順番で記録している。並べ替えが起きると番号がずれる。

### 3.16 E-2【低】Web インポート

**場所**: [geodatariversurveywebimporter.cpp](../libs/geodata/riversurvey/geodatariversurveywebimporter.cpp)

- 通信エラーのメッセージを出したあとも処理を続け、空の zip を展開しようとする。
- `m_mlitImporter->importInit(...)` の戻り値を見ずに `true` を返す。そのため、取り込みに失敗した場合やキャンセルした場合も、取り込みに進む。

### 3.17 F-1【低】断面がないデータのエクスポート

*.riv、ODN、LandXML の各エクスポータは、`headPoint()->nextPoint()` が nullptr かどうかを確かめずに参照している。断面が 1 つもないデータをエクスポートすると、異常終了する。HONRYU は確かめている。エクスポートの前に確かめ、エラーメッセージを出すようにする。

---

## 4. まとめ

1. *.riv の修正は、新しい内部データを追加したものではない。以前からある `leftShift`（左岸からの距離への変換値）を使い、「左岸からの距離 0 の点」を `#survey` の左岸座標として書き出すことで、形式を変えずに距離を保存できるようにした。*.riv から取り込んだデータでは、元のファイルの距離標の座標と距離がそのまま書き出される。
2. ODN、LandXML、CSV（HONRYU）のコードは変更していない。ODN は、Nb の付け替えの改善で間接的に正しくなった。一方、LandXML と HONRYU は距離の基準が *.riv とそろっていない（B-3）。
3. 今回の修正で *.riv の出力が `leftShift` に依存するようになったので、**`leftShift` が Undo やダイアログの「適用」で壊れる不具合（A-1）を、続けて直すことをおすすめする**。
4. 今回の件とは関係なく、LandXML のオフセット漏れ（B-1）は、オフセットを使うプロジェクトで座標が明らかにずれる。優先して直すべきである。

## 5. 修正の実施結果（2026-10-20）

本節は上記の調査時点からの変更を記録する。各不具合は個別コミットで修正する。

| ID | 修正内容 | 検証 |
|---|---|---|
| A-1 | シフト・拡幅コマンドで変更前後の `leftShift` を保存し、Undo/Redo で復元 | `distanceOriginUndo` の 8 ケース成功（シフト/拡幅 × 適用の有無 × OK/キャンセル。OK 後の Undo/Redo も検証） |
| B-1 | LandXML の左右岸の平面座標と TIN の全頂点にプロジェクトのオフセットを加算 | `landXmlCoordinates` の 2 ケース成功（ゼロ/非ゼロのオフセット。左右岸線の全端点と TIN 全頂点・標高を出力 XML から照合、許容誤差 0.51 mm） |
| B-3 | LandXML の横断距離を `position + leftShift` に変更し、左岸線形・測点距離も距離0の座標に統一。CSV/HONRYU に距離基準の選択を追加 | `honryuDistanceOrigins` の 12 ケース成功（CSV/テキスト × 新既定/従来方式 × 負/ゼロ/正の `leftShift`、無効な先頭点）。`landXmlDistanceOrigins` と B-1 のテストも成功 |
| B-2 | PVI の測点距離に対応する次の断面の標高を使用 | `landXmlProfileElevations` 成功。断面ごとに10 m異なる標高と傾斜を設定し、全 PVI の距離・補間標高を最終断面まで照合 |
| D-1 | RIV の列数・点数（2以上の整数）・4点番号（1始まり、範囲内かつ非減少）・距離/標高の組数・有限数値・左右岸座標の相違を検証。読み込み失敗と空データも明示的にエラー表示し、失敗時の仮データを解放 | `rivInputValidation` の 23 ケース成功。不正な19入力を拒否し、通常・4点あり・測線のみ・空行ありの4入力を許容。既存の338断面の RIV/CSV 往復テストも成功 |

B-3 の仕様はユーザー確認済み。CSV/HONRYU は画面と同じ距離を既定とし、従来の先頭有効点基準も選べる。LandXML の距離0点の標高は、有効な測量点から線形補間できる場合だけ使用する。全断面で求められない場合は Profile 全体を省略して警告する（範囲外への標高の外挿はしない）。断面データ・線形・TIN は引き続き出力する。§3.6 の共通関数抽出は、単純な `-leftShift` のため行っていない。RIV の符号制約は D-2 で解消する。

検証は Debug ビルド。今回の環境ではメインウィンドウ表示時に既存 VTK の FBO アサートが発生するため、描画に依存しない新規テストは `$env:IRIC_RS_TEST_HEADLESS='1'` でメインウィンドウを表示せず実行する。ダイアログの操作とプロジェクトの読み込み・終了は通常の Windows/Qt 経路を使用する（オフスクリーンプラグインではない）。
