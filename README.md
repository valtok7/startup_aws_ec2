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
make install
```

g++のインストール
```bash
# see https://mijinc0.github.io/blog/post/20190530_cpp_arm_cross_compile/
sudo apt install g++-aarch64-linux-gnu
```

~/.bash_profileを作成し、下記を記載する。bash起動時に読み込まれる。
```bash
# QEMUのライブラリの位置を指定
export QEMU_LD_PREFIX=/usr/aarch64-linux-gnu
```


## 実行
helloを実行する場合
```bash
qemu hello
```
