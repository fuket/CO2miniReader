# CO2miniReader
CO2-miniから温度とCO2濃度を取得するための開発者向けライブラリ。（非公式）

## 対象ハードウェア
(株)カスタム CO2モニター CO2-mini

## 動作環境
Windows

## 開発環境
Visual Studio Professional 2017

## 使い方
`CO2miniReader.sln`ファイルを開いて、ビルドしてください。

リリースフォルダ内にできる`Sample.exe`でセンサー値を取得できます。実行すると以下のように温度(temp)とCO2濃度(CO2)が表示されます。
```
> .\Sample.exe
temp: 18.1
co2 : 821.0
```

`Sample.exe`は、`CO2miniReader.h`をインクルードし、`CO2miniReader.lib`をリンクしています。
これらをご自身のプロジェクトで利用することで、センサーの値を利用したソフトを簡単に開発することができます。

## 利用ライブラリ
[hidapi](https://github.com/signal11/hidapi)

## 参考
* https://github.com/kurain/co2mini
* https://github.com/maddindeiss/co2-monitor
* https://hackaday.io/project/5301-reverse-engineering-a-low-cost-usb-co-monitor
