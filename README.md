# レムプレイヤー ソース

このリポジトリの mgba.wasm は mGBA（MPL-2.0）の64MB対応フォーク
https://github.com/liru55/mgba-celio-web （コミット c9470e570ccb2095e020edb2fd266858df8df95f）
を、clang（wasm32-wasip1）＋ wasi-libc でビルドしたものです。

- bridge.c … JavaScript から呼ぶための関数（ROM読み込み・1フレーム実行・映像/音声・セーブ・ステート）
- memory.c … anonymousMemoryMap を calloc で置き換える小さな実装
- build.sh / srcs.txt … コンパイルに使ったフラグと mGBA のソース一覧
- LICENSE-mGBA-MPL2.txt … mGBA のライセンス（MPL-2.0）

ページ本体（index.html）のJavaScriptはそのまま読めるソースです。
