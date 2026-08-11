# C++ 学びメモ

## 2026/08/11
- 拡張for文で `for(Bullet bullet : bullets)` と書くとコピーになる
- 元の要素を変更したいときは `for(Bullet& bullet : bullets)`
- 読むだけなら `for(const Bullet& bullet : bullets)`
- 今回のバグ：bulletActiveをtrueにしたのに、次の処理ではfalseのままだった

- 斜め移動では、移動の正規化が必要。早くなっちゃうため。
- 正規化は、ベクトルの長さを計算→0より大きいならlengthで割る。