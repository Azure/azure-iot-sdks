---
platform: mbed
device: gr-peach
language: c
---

Run a simple C sample on GR-PEACH device running Mbed
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Azure IoT Hub の設定](#Prerequisites)
-   [Step 2: デバイスの準備](#PrepareDevice)
-   [Step 3: サンプルプログラムのビルドと実行](#Build)

<a name="Introduction"></a>
# はじめに 

本手順書は、GR-PEACH を用いて Azure IoT Hub に接続する手順を示したものである。手順は以下のステップに分かれる。 

-   Azure IoT Hub の設定 
-   デバイスのレジストリ登録 
-   サンプルプログラムのビルドと実行 
 
本手順を実行するにあたり、以下のものを用意する。 

-   GR-PEACH 
-   mbed 開発環境 
-   インターネット環境 
-   SSH クライアント（本手順では Tera Tarm を使用し説明する） 
-   Microsoft Azure アカウント 
 

<a name="Prerequisites"></a>
# Azure IoT Hub の設定 

## セットアップ 

以下の URL を参照し、Azure IoT Hub の設定を行う。

<https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md> 

## Device Explorer のインストール
 
以下の URL を参照し、Device Explorer をインストールし、デバイス名を登録する。 

<https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md> 


<a name="PrepareDevice"></a>
# デバイスの準備 

## インターネット接続 
GR-PEACH にイーサネットケーブルを接続し、インターネットに接続する。 

![GRPeach\_image01](media/mbed-gr-peach-c-image01.png)
 
## microUSB ケーブルの接続 
GR-PEACH のハードリセットボタンの横にある microUSB 端子と、開発 PC の USB ポートを microUSB ケーブルで接続する。 

![GRPeach\_image02](media/mbed-gr-peach-c-image02.png)
 
 
<a name="Build"></a>
# サンプルプログラムのビルドと実行 
 
## サンプルプログラムのインポート 

以下の URL を参照し、自身の mbed 開発者ページにサンプルプログラムをインポートし、接続文字列を置き換える。 
 
<https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/mbed-renesasgr-peach-c.md#create-mbed-project-and-import-the-sample-code>

## ビルド 

以下の URL を参照し、ビルドを行う。 
ビルドが成功すると自動的にバイナリファイルがダウンロードされる。 
エクスプローラを開き、外付けUSBドライブとして認識されているMBEDドライブにダウンロードされたバイナリファイルをドラッグ＆ドロップしコピーする。 
 
<https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/mbed-renesasgr-peach-c.md#build-and-run-the-program> 
 
## 実行 

-   MBEDドライブにバイナリファイルが書き込まれたら、GR-PEACHのハードリセットボタンを押し、実行する。 
Tera Term や Device Explorer を用いて Azure IoT Hub とのデータ通信をモニタする。 

    ![GRPeach\_image03](media/mbed-gr-peach-c-image03.png)
 
-   補足 Tera Term によるモニタデバイスマネージャーを開き、GR-PEACH が接続されている COM ポート番号を確認する。 

    ![GRPeach\_image04](media/mbed-gr-peach-c-image04.png)

    ### Tera Term を起動する

    -   ファイル(F)タブの新しい接続をクリックする。 

        ![GRPeach\_image05](media/mbed-gr-peach-c-image05.png)
  
    -   シリアルポートを選択し、GR-PEACH が接続されている COM ポートを選び、OK をクリックする。 

        ![GRPeach\_image06](media/mbed-gr-peach-c-image06.png)
 
    -   接続(S)タブのシリアルポートをクリックする。 

        ![GRPeach\_image07](media/mbed-gr-peach-c-image07.png)

    -   ボーレートを 9600 に設定し、OK をクリックする。 

        ![GRPeach\_image08](media/mbed-gr-peach-c-image08.png)
 
-   GR-PEACH のハードリセットボタンを押すとプログラムが再実行され、結果が出力される。 

    ![GRPeach\_image09](media/mbed-gr-peach-c-image09.png)
