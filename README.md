# Startup AWS EC2

AWS EC2の初期設定

# ssh keyの設定
ローカルに置いてある公開鍵をコピーする
~/.ssh/aws_ssh_key.pub
の中身をコピーして、ホスト(EC2)の
~/.ssh/authorized_keys
にコピーする。

ローカルからssh接続するときは
~/.ssh/aws_ssh_key
を使用する
例
```bash
ssh -i "aws_ssh_key" ec2-user@ec2-34-228-79-195.compute-1.amazonaws.com
```
接続先はawsコンソールのインスタンスを確認する。

# ssh接続が自動切断されないようにする
/etc/ssh/sshd_configを以下のように変更する。
```bash
#ClientAliveInterval 0
#ClientAliveCountMax 3
```
↓
```bash
ClientAliveInterval 60
ClientAliveCountMax 120
```
これで放っておいても120分切断されないようになる。

sshdを再起動する。
```bash
sudo service sshd restart
```

# swapの作成
デフォルトではswapファイルが作成されていない。swapを作成してメモリの少なさをカバーする。

```bash
dd if=/dev/zero of=/swap bs=1M count=2048
mkswap /swap
swapon /swap
```

このままではインスタンスを再起動した時にはまたスワップ領域が無くなってます。毎回起動時に自動でスワップ領域が確保されるように/etc/fstabに以下の記述を加えます。
```bash
/swap          swap                    swap    defaults        0 0
```

メモリの状態は以下のコマンドで確認できる。
```bash
free
```

# Git, docker, gcc, g++のインストール

```bash
sudo yum install git
sudo yum install docker
sudo yum install gcc
sudo yum install gcc-c++
```

# OpenSSLのインストール

```bash
sudo yum install libssl-dev
```

# CMakeのインストール

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc4/cmake-3.23.0-rc4.tar.gz
tar -zxvf cmake-3.23.0-rc4.tar.gz
cd cmake-3.23.0-rc4/
./bootstrup
make
sudo make install
```

# GoogleTestのインストール

```bash
git clone https://github.com/google/googletest.git -b release-1.11.0
cd googletest        # Main directory of the cloned repository.
mkdir build          # Create a directory to hold the build output.
cd build
cmake ..             # Generate native build scripts for GoogleTest.
make
sudo make install    # Install in /usr/local/ by default
```

# Requirement

"hoge"を動かすのに必要なライブラリなどを列挙する

* huga 3.5.2
* hogehuga 1.0.2

# Installation

Requirementで列挙したライブラリなどのインストール方法を説明する

```bash
pip install huga_package
```

# Usage

DEMOの実行方法など、"hoge"の基本的な使い方を説明する

```bash
git clone https://github.com/hoge/~
cd examples
python demo.py
```

# Note

注意点などがあれば書く

# Author

作成情報を列挙する

* 作成者
* 所属
* E-mail

# License
ライセンスを明示する

"hoge" is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

社内向けなら社外秘であることを明示してる

"hoge" is Confidential.