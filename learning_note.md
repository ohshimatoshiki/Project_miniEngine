# C++ 学びメモ

## 2026/08/11
- 拡張for文で `for(Bullet bullet : bullets)` と書くとコピーになる
- 元の要素を変更したいときは `for(Bullet& bullet : bullets)`
- 読むだけなら `for(const Bullet& bullet : bullets)`
- 今回のバグ：bulletActiveをtrueにしたのに、次の処理ではfalseのままだった

- 斜め移動では、移動の正規化が必要。早くなっちゃうため。
- 正規化は、ベクトルの長さを計算→0より大きいならlengthで割る。

## 2026/08/12
- 参照渡しの書き方
- ‵Bullet (&bullets)[maxBullets]‵引数を左のようにする
- これにより、Bullet型の配列を参照渡しできる
- const は宣言後に値を変更しない
- constexpr はコンパイル時から値を変更しない
- constは宣言されるまでは何かわからんくてもいい。constexprは定数みたいな扱い
- const 型& 変数名 で、この関数内ではなにも変更しませんよーになり、参照渡しだからメモリも軽い。