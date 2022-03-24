# Startup AWS EC2

AWS EC2の初期設定

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

## OpenSSLのインストール

```bash
sudo yum install openssl-devel.x86_64
```

## CMakeのインストール

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc4/cmake-3.23.0-rc4.tar.gz
tar -zxvf cmake-3.23.0-rc4.tar.gz
cd cmake-3.23.0-rc4/
./bootstrup
make
sudo make install
```

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

## Docker-composeのインストール

```bash
sudo curl -L "https://github.com/docker/compose/releases/download/1.26.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
sudo ln -s /usr/local/bin/docker-compose /usr/bin/docker-compose
```




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