# Ubuntu Reference

## Package

* /etc/apt/sources.list
ディストリビューションのアップデート等について
例
```bash
deb http://archive.ubuntu.com/ubuntu/ focal main restricted
```
"deb" 行がバイナリーパッケージのための定義です。
"deb-src" 行がソースパッケージのための定義です。一番目の引数は、Debian アーカイブの root URL です。
二番目の引数は、スイーツ名かコード名のどちらかで与えられるディストリビューション名です。
三番目次の引数は、Debian アーカイブの中の有効なアーカイブのエリア名のリストです。

