# ホスト環境構築編

## WSL

### ubuntuのインストール
インストール
```bash
wsl --install -d Ubuntu
```
インストールされているディストリビューションの確認
```bash
wsl --list
```
起動しているディストリビューションの確認
```bash
wsl -l -v
```
Ubuntuバージョン確認
```bash
cat /etc/lsb-release
```



## AWS

### VPC
IPv4 CIDRブロックを指定
VCPのネットワーク範囲を決める
192.168.1.0/24だと168.192.1.0～255までの範囲となる

### インターネットゲートウェイ
インターネットへの出入口をVPCにつける
インターネットゲートウェイを作成して、VPCにアタッチから先ほど作成したVPCを選択する。

### ルートテーブル
先ほど作成したVPCを選択して作成
ルートは
129.168.1.0/24  local  # デフォルトで作成。VPC内のすべてを宛先としたローカル用ルート
0.0.0.0/0   igw-xxx   # 新規追加。インターネット全体を宛先としたルート。ターゲットには先ほど作成したインターネットゲートウェイを指定

### サブネット
VPCの中に1つ以上のサブネットを作成する。インスタンスはサブネットの中に配置する必要があるため。
先ほど作成したVPCを選択
アベイラビリティゾーンを選択（どこでもよい）
IPv4 CIDRブロックを指定
サブネットのネットワーク範囲を決める
192.168.1.0/25だと168.192.1.0～127までの範囲となる
ルートテーブルの編集で先ほど作成したルートテーブルを指定する

### EC2インスタンス
インスタンスの詳細設定で
・ネットワークに先ほど作成したVPCを選択する
・サブネットには先ほど作成したサブネットが設定されている
・自動割り当てパブリックIPを有効にすると、インターネット側のIPが固定される。

### ログイン
インスタンス作成時に作成したssh鍵をローカルの.sshにコピーして、起動。
```bash
cp aws-ssh.pem ~/.ssh/
ssh -i "aws-ssh.pem" ec2-user@13.230.95.18
```


## ssh keyの設定（自分で鍵を作成する場合）
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

## ssh接続が自動切断されないようにする(Amazon Linux)
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

## swapの作成(Amazon Linux)
デフォルトではswapファイルが作成されていない。swapを作成してメモリの少なさをカバーする。

```bash
sudo dd if=/dev/zero of=/swap bs=1M count=2048   # 結構時間がかかります
sudo mkswap /swap
sudo swapon /swap
```

このままではインスタンスを再起動した時にはまたスワップ領域が無くなってます。毎回起動時に自動でスワップ領域が確保されるように/etc/fstabに以下の記述を加えます。
```bash
/swap          swap                    swap    defaults        0 0
```

メモリの状態は以下のコマンドで確認できる。
```bash
free
```

## Development Tools(gccとか), dockerのインストール

** Amazon Linux **
```bash
sudo yum groupinstall "Development Tools"
sudo yum install docker
```

** Ubuntu **
```bash
sudo apt update && sudo apt install build-essential
sudo apt-get install \
    ca-certificates \
    curl \
    gnupg \
    lsb-release
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
sudo apt-get install docker-ce docker-ce-cli containerd.io
```

## gitconfigの設定

```bash
git config --global user.name "username"
git config --global user.email emailaddress@address.com
```

## Dockerの起動

サービス起動
```bash
sudo service docker start
```

sudo を使用せずに Docker コマンドを実行できるように、ec2-user (Amazon Linux) を docker グループに追加
```bash
sudo usermod -a -G docker ec2-user
```

一度ログアウトして再度ログインする。
そしてDockerを動かせることを確認する。
```bash
docker run hello-world
```

## OpenSSLのインストール（CMakeに必要）

** Amazon Linux **
```bash
sudo yum install openssl-devel.x86_64
```

** Ubuntu **
```bash
sudo apt install libssl-dev
```

## CMakeのインストール

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc4/cmake-3.23.0-rc4.tar.gz
tar -zxvf cmake-3.23.0-rc4.tar.gz
cd cmake-3.23.0-rc4/
./bootstrup
make
sudo make install
cd ..
rm -r cmake-3.23.0-rc4
rm cmake-3.23.0-rc4.tar.gz
```

## Docker-composeのインストール

```bash
sudo curl -L "https://github.com/docker/compose/releases/download/1.26.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
sudo ln -s /usr/local/bin/docker-compose /usr/bin/docker-compose
```

# コンテナ構築編
https://github.com/valtok7/docker-env.git

ディレクトリ構成
```
├── .devcontainer
│   ├── devcontainer.json
│   ├── Dockerfile
│   ├── requirements.txt
│   └── run_docker_env.sh
├── .vscode
│   └── c_cpp_properties.json
└───.gitignore
```

Dockerfile
```docker
# base image
FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

# install software
# ベースイメージにはsudoが入っていないので注意
RUN apt-get update && \
    apt-get install -y build-essential gdb cmake libssl-dev clang python3-pip sudo git iputils-ping net-tools python3-pip
## googletest
RUN git clone https://github.com/google/googletest.git -b release-1.11.0 \
    && cd googletest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && sudo make install    # Install in /usr/local/ by default

# add user
# Vscodeから起動した場合UID,GID,USERNAMEはdevcontainer.jsonに記載の値に書き換えられる。直接docker runで起動したときはこの値が使用される。
ARG UID=1000
ARG GID=1000
ARG USERNAME=user
RUN groupadd --gid $GID USERNAME \
    && useradd -m -s /bin/bash --uid $UID --gid $GID $USERNAME \
    && echo $USERNAME:$USERNAME | chpasswd \
    && gpasswd -a $USERNAME sudo
USER $USERNAME
```

docker-compose.yml
```yml
version: "3.3"
services:
  develop:
    build:
      context: ..
      dockerfile: Dockerfile
    image: develop
    container_name: develop
    volumes:
      - type: bind
        source: .
        target: /workspace
    stdin_open: true
    tty: true
    working_dir: /workspace
    command: /bin/sh -c "while sleep 1000; do :; done"
```

起動用スクリプトとしてrun_docker_env.shを用意。
run_docker_env.sh
```bash
sudo docker image build -t docker_env .
sudo docker run -v /home/ito/work/docker-env:/workspace/docker_env -w /workspace/docker_env -it docker_env
```
（作成時はchmod +x run_docker_env.shをしておくこと）

c_cpp_properties.json
```json
{
	"name": "Ubuntu",
	"build": {
    // Docker-composeを使うとvscodeのExtensionの自動インストールが行われない（バグかもしれない）。そのためDockerfileから起動する。
		"dockerfile": "Dockerfile",
    // WSL Ubuntuの初期ユーザーのUser IDとGroup IDを使う。UIDとGUDがあっていればUsernameが異なっていてもファイルのパーミッションが同一になるので、コンテナ上で編集したファイルがWSL上で編集できないということがなくなる。
		"args": {
			"UID": "1000",
			"GID": "1000",
			"USERNAME": "user"
		}
	},
  // docker runに渡す引数
	"runArgs": [
    // IP系の操作をするために必要
		"--privileged"
	],
	"settings": {
	"settings": {
		"C_Cpp.default.cppStandard": "c++17",
		"C_Cpp.clang_format_sortIncludes": true,
		"C_Cpp.clang_format_style": "{BasedOnStyle: Google, IndentWidth: 4, AccessModifierOffset: -4}",
		"editor.formatOnSave": true,
		"python.languageServer": "Pylance",
		"python.pythonPath": "/usr/bin/python3",
		"python.linting.flake8Args": [
			"--max-line-length", // 1 行あたりの文字数を 110 に設定
			"110"
		],
		"python.formatting.provider": "autopep8",
		"python.formatting.autopep8Args": [
			"--max-line-length", // 1 行あたりの文字数を 110 に設定
			"110"
		],
	},
	"extensions": [
		"ms-vscode.cpptools-extension-pack",
		"visualstudioexptteam.vscodeintellicode",
		"donjayamanne.git-extension-pack",
		"ms-python.python",
		"ms-python.vscode-pylance",
		"shardulm94.trailing-spaces",
		"vscodevim.vim"
	],
	"postCreateCommand": "pip3 install -r ./devcontainer/requirements.txt",
	"remoteUser": "user"
}
```

requirements.txt
```
autopep8==1.5.4
flake8==3.8.4
mccabe==0.6.1
numpy==1.19.4
pycodestyle==2.6.0
pyflakes==2.2.0
toml==0.10.2
```
requirements.txt は Python のパッケージ管理に使うファイル

c_cpp_properties.json
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [],
            // clangにしないとvscodeのインテリセンスのエラー表示が誤ったものになる（gccのstlがclang的にコンパイルできないとしてエラーにさせられる？）
            "compilerPath": "/usr/bin/clang++",
            "cStandard": "gnu17",
            // clangにしている理由は上記
            "intelliSenseMode": "linux-clang-x64",
            "cppStandard": "c++17"
        }
    ],
    "version": 4
}
```

## コンテナの操作
コンテナのビルドと起動
```bash
docker-compose up -d
```
-dはバックグラウンドで実行の意味

コンテナに入る
```bash
docker container exec -it (コンテナ名) /bin/bash
```

コンテナの確認
```bash
docker ps -a
```

コンテナの削除
```bash
docker rm (コンテナID)
```
コンテナIDはdocker ps -aで確認できる


## VS Codeを使用したコンテナの起動
Dockerfileのあるディレクトリでcode .を実行
左下の緑色の部分をクリックしてReopen in containerを選択









## GoogleTestのインストール

```bash
git clone https://github.com/google/googletest.git -b release-1.11.0
cd googletest        # Main directory of the cloned repository.
mkdir build          # Create a directory to hold the build output.
cd build
cmake ..             # Generate native build scripts for GoogleTest.
make
sudo make install    # Install in /usr/local/ by default
```

# Aarch64

## 環境構築
QEMUのインストール
```bash
# QEMUのconfigに必要
sudo apt install ninja-build
sudo apt install pkg-config
sudo apt install libglib2.0-dev
# see https://www.qemu.org/download/
git clone https://gitlab.com/qemu-project/qemu.git
cd qemu
git submodule init
git submodule update --recursive
# aarch64用にするため--target-list=aarch64-linux-userを指定
./configure --target-list=aarch64-linux-user
make
sudo make install
```

aarch64用g++のインストール
```bash
# see https://mijinc0.github.io/blog/post/20190530_cpp_arm_cross_compile/
sudo apt install g++-aarch64-linux-gnu
```

aarch64用gdbのインストール
```bash
# see https://qiita.com/takeoverjp/items/5df8e17f0c361ecd3563
sudo apt install gdb-multiarch
```


* bashの場合
~/.bash_profileを作成し、下記を記載する。bash起動時に読み込まれる。
```bash
export QEMU_LD_PREFIX=/usr/aarch64-linux-gnu
```
* fishの場合
環境変数の永続化を行う
```bash
export set -Ux QEMU_LD_PREFIX /usr/aarch64-linux-gnu
```


## 実行
helloを実行する場合
```bash
qemu-aarch64 hello
```

## デバッグ実行
helloを実行する場合
```bash
qemu-aarch64 -g 1111 hello
```
別のシェルを起動してgdbを実行
```bash
gdb-multiarch
```

gdbを操作
```gdb
file hello
target remote localhost:1111
b main    #ブレイクポイントをmainに設定
c         #Continue
```

shared libraryのデバッグ
soファイルは通常LD_LIBRARY_PATHの通った場所に置く必要があるが、QEMUの場合は/usr/aarch64-linux-gnu/libに置く。
```gdb
(gdb) file hello
(gdb) set solib-search-path ./     # 共有ライブラリのディレクトリ（シンボル読み込み）
(gdb) directory ../../calc/Debug   # 共有ライブラリのディレクトリを追加（ソース読み込み）
(gdb) target remote localhost:1111
(gdb) b main
(gdb) c
(gdb) info shared
From                To                  Syms Read   Shared Object Library
0x000000550083f7c0  0x000000550083f954  Yes         /home/ito/work/hello_aarch64/hello_aarch64/Debug/libcalc.so
                                        No          /lib/libstdc++.so.6
                                        No          /lib/libm.so.6
                                        No          /lib/libgcc_s.so.1
                                        No          /lib/libc.so.6
                                        No          /lib/ld-linux-aarch64.so.1

```
一連の処理を.gdbinitに書いておけば起動時に自動化できる。


~/.gdbinitサンプル
```gdb
# コマンド履歴を保存する
set history save on
set history size 10000
set history filename ~/.gdb_history

# listコマンドで表示する行数
set listsize 25

# 配列の要素を全て表示する
set print elements 0

# 構造体のメンバを1行ずつ表示できる
set print pretty on

# Auto load local .gdbinit
set auto-load local-gdbinit
set auto-load safe-path ~/work/hello_aarch64/hello_aarch64/Debug/  # 読み込みを許可する
```

## gdb-dashboard
https://github.com/cyrus-and/gdb-dashboard
.gdbinitを~/に置く。

~/.config/gdb-dashboard/initに初期設定を書く
例えば以下のようにする。
```gdb
# コマンド履歴を保存する
set history save on
set history size 10000
set history filename ~/.gdb_history

# listコマンドで表示する行数
set listsize 25

# 配列の要素を全て表示する
set print elements 0

# 構造体のメンバを1行ずつ表示できる
set print pretty on

# Auto load local .gdbinit
set auto-load local-gdbinit
set auto-load safe-path ~/work/hello_aarch64/hello_aarch64/Debug/

# GDB dashboard configuration
dashboard -layout !assembly breakpoints expressions !history !memory !registers source stack !threads variables
```
~/work/hello_aarch64/hello_aarch64/Debug/.gdbinitにローカルプロジェクトの初期設定を書く。
例えば以下のようにする。
```gdb
file hello_aarch64.a
set solib-search-path ./
directory ../../calc/Debug
target remote localhost:1111
b main
c
info shared
```

パスの差し替え
set substitute-path from-path  to-path


参考
https://retrotecture.jp/cortexm/gdb.html#conffile
http://www.fos.kuis.kyoto-u.ac.jp/le2soft/siryo-html/node49.html


ここにmultiarchのデバッグ方法が書かれている
https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/DebuggingGuide.pdf

## Window上のEclipseからアタッチ
msys64をインストールする(pacmanを使用するため)

gdb-multiarchをインストールする
msysコンソール上から下記を実施
```bash
pacman -S mingw-w64-x86_64-gdb-multiarch
```

eclipse
デバッグ構成 - GDBハードウェアデバッグ - デバッガー - GDB Command
C:\msys64\mingw64\bin\gdb-multiarch.exe
デバッグ構成 - GDBハードウェアデバッグ - デバッガー - GDB Connection Setting
接続対象のIPアドレス:ポート番号を指定
例：172.21.65.177:1111

デバッグを押すと開始する

# fishの導入

## 環境構築
インストール
```bash
sudo apt-add-repository ppa:fish-shell/release-3
sudo apt update
sudo apt install fish
```

標準shellにする
```bash
chsh -s $(which fish)
# chsh -s /bin/bash
# echo $SHELL 現在使用しているシェルの表示
# cat /etc/shells 使用できるシェルの一覧
```
一時的にfishをshellにする場合
```bash
fish
```

Oh-my-fish(fish plug-in manager)
```bash
curl -L https://get.oh-my.fish | fish
curl https://git.io/fisher --create-dirs -sLo ~/.config/fish/functions/fisher.fish
```

プロンプトの変更
```bash
fisher install oh-my-fish/theme-bobthefish
```

peco（ctrl-rでコマンド履歴を検索）
```bash
brew install peco                                                                               
fisher install oh-my-fish/plugin-peco                                                               
```

ctrl+wでpecoを有効にするため、下記を追加する
~/.config/fish/config.fish   
```bash
function fish_user_key_bindings
  bind \cw 'peco_select_history (commandline -b)'
end
```

bass（bashラッパー）
```bash
fisher install edc/bass
```

fzf（ファイル検索など）
```bash
sudo apt install fzf
fisher install jethrokuan/fzf
```

## 使い方
ctrl+w
コマンド補完

prevd, nextd
ディレクトリ移動

bash互換1
bass 'bash用コマンド'
例
bass 'echo $(date)'

bash互換2
シェルスクリプトの先頭に

ctrl+o
ファイル検索

ctrl+r
コマンド履歴検索

alt+c
サブディレクトリcd

alt+shift+c
非表示含めたサブディレクトリcd

alt
デフォルトのエディター($EDITOR)を使用してファイルを開く


# Windows Eclipse

## GNU Toolchain for Aarch64
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads
AArch64 GNU/Linux target (aarch64-none-linux-gnu)をダウンロードする。

c:\tool\の下などに解答する。

## Eclipseのクロスコンパイル設定
プロジェクトのプロパティを開く
C/C++ビルド - ツール・チェーン・エディター - 現在のツールチェーン
を
Cross GCCに変更する。

C/C++ビルド - 設定 - Cross Setting - 接頭辞
を"aarch64-none-linux-gnu-"とする。
パスを"C:\Tool\gcc-arm-10.3-2021.07-mingw-w64-i686-aarch64-none-linux-gnu\bin"にする

C/C++ビルド - 設定 - ビルド成果物 - 成果物の拡張子
をaにしておくとわかりやすい。

ビルドしてwslの上にコピーする。

chmod +x hello.a

qemu-aarch64 hello.a
みたいにすると実行できる。

# MYSQL

インストール
```bash
sudo apt install mysql-server
```
サービス開始・接続
```bash
sudo service mysql start
sudo mysql -u root -p
```
データベースの作成
```sql
mysql> create database <データベース名>;
```
外部接続用のユーザーを追加する
localhost以外で接続する場合(外部からの接続や、同一マシン内でもマシン名やIPアドレスで接続)、追加で以下の設定が必要です。
```sql
mysql> CREATE USER '<ユーザー>'@<ホスト> IDENTIFIED WITH mysql_native_password BY '<パスワード>';
mysql> GRANT ALL PRIVILEGES ON <データベース>.<テーブル> TO '<ユーザー>'@<ホスト>;
mysql> FLUSH PRIVILEGES;
```
ユーザーが追加されたことを確認する
'root'@'%'を追加した場合
```sql
mysql> select user,host from mysql.user;
+------------------+-----------+
| user             | host      |
+------------------+-----------+
| root             | %         |
| debian-sys-maint | localhost |
| mysql.infoschema | localhost |
| mysql.session    | localhost |
| mysql.sys        | localhost |
| root             | localhost |
+------------------+-----------+
mysql> exit
```
/etc/mysql/mysql.conf.d/mysqld.cnfの次の行をコメントアウトする
または接続を許可するIPアドレスに変更する
```
bind-address        = 127.0.0.1
```
MySQLをリスタートする
```bash
sudo service mysql restart
```
ポートが開いているか確認
```bash
namp <IP address>

PORT     STATE SERVICE
3306/tcp open  mysql
```

参考
https://www.yokoweb.net/2020/08/16/ubuntu-20_04-server-mysql/#toc2
https://moewe-net.com/database/mysql-install-on-ubuntu

# 自作OS

https://qiita.com/hotchpotch/items/9cdf6233c2b09e57aac1

## gdb
https://forest1040.hatenadiary.org/entry/20120921/1348191532
パッチを当てて、gdbのトップのMakefileのCLAGSに-z muldefsを足すことでビルドが通った。
↓
gdb-12.1で
mkdir build
cd build
../configure --target=h8300-elf --disable-nls --disable-werror
とすればビルドが通る

gdbを使用した組み込みソフトのシミュレーション
https://www.simulationroom999.com/blog/gdb-iss1/

## USB-シリアル変換ケーブルを使えるようにする
https://qiita.com/baggio/items/28c13ed8ac09fc7ebdf1

sudo chmod g+wr /dev/ttyUSB0
sudo chmod o+wr /dev/ttyUSB0
を行う必要あり

## xmodemでのファイル転送
sudo screen /dev/ttyUSB0
kozos > load
ctrl+a, :
exec !! sx (filename)

screenを終了するときは
ctrl+a, :
quit

## Makefile
https://ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html


ブロックチェーン技術を活用することで、コピーが容易なデジタルデータに対し、唯一無二な資産的価値を付与し、新たな売買市場を生み出す技術として注目を浴びている「NFT（Non-Fungible Token：非代替性トークン）」。

2017年にイーサリアムブロックチェーン上で誕生した「CryptoKitties」というゲームに端を発する。

NyanCat
https://courrier.jp/news/archives/237102/

2021年3月には、Twitter創業者のジャック・ドーシー氏の出品した同氏の初ツイートが約3億円で落札。
https://artnewsjapan.com/news_criticism/article/176

イーロン・マスク氏が出品した音楽作品には約1億円の値が付いた。
Beeple「The First 5000 Days」 70億円

NFTと新規通貨を絡めたゲームが乱発されている。


X to earn
プレイすることで仮想通貨を獲得できるゲーム
STEPN（ステップン）
睡眠の時間や質によって報酬が得られる「Sleep to Earn」、学んで稼げる「Learn to Earn」や飲食することによって稼ぐ「Eat to Earn」「Drink to Earn」
https://tokenknowledge.com/stepn-move-to-earn/

ユガ・ラボ
2022/4/30 開発中のメタバースプロジェクト「Otherside」における土地を表すNFT「Otherdeed」の販売を完了。その売り上げは、2億8500万ドル（350億円）にも達した。

The Sandbox（ザ・サンドボックス）
世界に4000万人超の利用者
完売まで、わずか1分44秒
最低30万円。1月に194万円。FacebookがMetaになった。

通貨発行利益、先行者利益を

NFTは「100％、Greater fool theoryに基づいている」とゲイツ氏。Greater fool theoryとは、金融業界で、過大評価された資産でも、自分より愚かな誰かにさらに高値で転売できると踏んで購入することを指す。現実的な価値との乖離が大きくなりすぎれば、価格は大幅に下落する。

https://artnewsjapan.com/news_criticism/article/176
シーナエスタビ。60億円で売って30億円を寄付すると言ってオークションにかけたが、150万円。



https://www.desultoryquest.com/blog/using-gcc-and-clang-with-eclipse-on-windows/

https://www.eclipse.org/forums/index.php/t/1097469/

Clang
https://iwakwak.hatenablog.com/entry/2020/01/16/113714
https://packages.msys2.org/package/mingw-w64-x86_64-lld?repo=mingw64
https://qiita.com/syoyo/items/a09d650bbe55c857ad2b

MSYS2
https://qiita.com/Ted-HM/items/4f2feb9fdacb6c72083c

Eclipse with CMake & Ninja
https://wiki.gnucash.org/wiki/Eclipse#With_CMake_.26_Ninja