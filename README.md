# SoraVoice (Lite)

This project's objective is to bring voice acting to the PC versions of *Sora/Zero/Ao no Kiseki* from the
*Evolution* version of these games.

All information about this project can be found at [SoraVoice (Lite)](https://github.com/ZhenjianYang/SoraVoice).

**NOTE**: This project is licensed under the GPLv3. You MUST copy, distribute and/or modify any code or binaries
from this project under this license. See [LICENSE](https://github.com/ZhenjianYang/SoraVoice/blob/master/LICENSE)
for details. 

### About **Lite**

This is a new (**lite**) version of the original [SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated), by
removing some uncommon used features, it has better compatibility and performance. Not like the original one, **game
update WON'T break the lite one** (hopefully).

**If you are playing _Trails in the Sky_ series, please use this lite version. Since the original
[SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated) is no longer under maintenance, and doesn't support
this series with latest updates**.

## How to let it work

Things you need:

- [The Game](#the-game)
- [SoraVoice (Lite)](#soravoice-lite-1)
- [Voice Scripts](#voice-files)
- [Voice files](#voice-files)

### The Game

Games brought from platforms listed below are supported. Games from other platforms (e.g. Wegame) may not be supported.

|Game Title                    |Publisher | Language         |Platform
|------------------------------|----------|------------------|------
|*Zero no Kiseki*              |Joyoland  |Chinese Simplified|Joyoland/Cubejoy
|*Ao no Kiseki*                |Joyoland  |Chinese Simplified|Joyoland/Cubejoy
|*Sora no Kiseki FC*           |YLT       |Chinese Simplified|
|*Sora no Kiseki SC*           |YLT       |Chinese Simplified|
|*Sora no Kiseki the 3rd*      |YLT       |Chinese Simplified|
|*Zero no Kiseki*              |Falcom    |Japanese          |
|*Trails in the Sky FC*        |Xseed     |Chinese Simplified|Steam, [ED6-FC-Steam-CN](https://github.com/Ouroboros/ED6-FC-Steam-CN)
|*Trails in the Sky FC*        |Xseed     |English           |Steam/GOG/Humble
|*Trails in the Sky SC*        |Xseed     |English           |Steam/GOG/Humble
|*Trails in the Sky the 3rd*   |Xseed     |English           |Steam/GOG/Humble

### SoraVoice (Lite)

Download the latest file at [Release](https://github.com/ZhenjianYang/SoraVoice/releases/latest), extract `dinput8.dll`
and folders `voice` & `dll` to your game's installation folder.

#### About `dsound.dll`

For *Sora no Kiseki*/*Trails in the Sky*, you can use `dsound.dll` instead of `dinput8.dll`, in case you couldn't use the
latter for some reasons (e.g. another MOD also uses `dinput8.dll`). But do NOT try to copy both of them to your game's
folder.

### Voice Scripts

**Voice Scripts** are at the very core of the patch, as they call the needed **Voice Files** line by line. They contain
all the dialogues, and because of that, obviously, each set of **Voice Scripts** is tied to a specific version of the
games.

We have two projects related to **Voice Scripts**:
- [ZeroAoVoiceScripts](https://github.com/ZhenjianYang/ZeroAoVoiceScripts/releases/latest), for *Zero / Ao no Kiseki*.    
- [SoraVoiceScripts](https://github.com/ZhenjianYang/SoraVoiceScripts/releases/latest), for *Sora no Kiseki* / *Trails
in the Sky*. 

Download the latest files for your games & languages and extract the whole folder `scena` into the folder `voice/`
extracted in the previous steps. Supported languages for each game are also listed at above links.

### Voice files

We won't provide any voice files, they can be only extracted from the *Evolution* edition games (PSVita).
Steps:

#### 1. Dump the game's image   
   You may need a copy of the *Evolution* edition game and a PSVita with Hankaku installed to do this.   
   Tutorials can be found somewhere else.

#### 2. Extract data.psarc   
   Drag & Drop `<Evolution Game Folder>/gamedata/data.psarc` on [PSArcTool](https://github.com/periander/PSArcTool).    
Or if you have `psarc.exe` from Sony's PS3 SDK, use this command: `psarc.exe extract data.psarc`   
   **Note**: If you find data0.psarc, data1.psarc, ... in the same folder with data.psarc, then extract them all the
   same way. 

#### 3. Convert at9 files to ogg files   
  - **Tools needed:**   
    - `at9tool.exe`, it can only be found in Sony's PS3 SDK.   
    - [`oggenc2`](http://www.rarewares.org/ogg-oggenc.php)  

1. Create a folder `at9`, and copy (or cut if you like) the extracted folder `data/talk` into it.   
- **NOTE**: For **_Sora no Kiseki_**/**_Trails in the Sky_** series, you also to need copy `data/bgm/arrange/ed6501.at9`(or
`data/bgm/ed6501.at9`) into `at9`. And, please keep in mind that for **_FC_**, **_SC_** and **_the 3rd_**, each game has
its own `ed6501.at9`.

2. Create `Convert.bat` with these contents:   
~~~
@echo off
mkdir wav
mkdir ogg
for /f "delims=" %%i in ('dir /s /b /a-d at9\*.at9') do (
title converting %%~ni.at9
at9tool.exe -d -repeat 1 "%%i" "wav\%%~ni.wav"
oggenc2.exe -Q -q 6.00 -n "ogg\%%~ni.ogg" "wav\%%~ni.wav"
)
~~~
- **NOTE**: `-q 6.00` sets ogg quality to 6.00, you can choose another value between -2 and 10 (higher value means higher
quality and larger filesize). 

3. Put `at9tool.exe`, `oggenc2.exe` and `Convert.bat` together with the folder `at9`(not in the folder), then double click
`Convert.bat`.   
- **NOTE**: This step may take a very long time, depending on your PC specs, as there are tenth of thousands of files to
encode. Please, be patient.   

Then, you will get **Voice Files** in the `ogg/` folder.

#### 4. Copy folder `ogg` into folder `voice/`

### Launch the game

If all goes well, you will see an information of SoraVoice (Lite) in the title bar after launching the game.

## About the configuration file

After launching the game, configuration file `voice/ed_voice.ini` will be automatically generated. Settings listed below:
|                  |                                                |
|------------------|-------------------------------------------------
|ShowInfo          | Show information of SoraVoice (Lite) in the title bar
|Volume            | Volume: 0 ~ 100
|DisableTextSe     | Disable dialog text beep SE. (Voiced lines only)
|DisableDialogSe   | Disable dialog closing SE.  (Voiced lines only) 
|DisableAoOriVoice | Disable original scenario voice (Ao only)


### Removed features

Comparing with the original [SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated), these features are removed:
- Auto dialogs
- Configuration Hot Keys
- Pop up information

## External libraries used in this project   
-   [GoogleTest](https://github.com/google/googletest)
-   [Ogg](https://github.com/xiph/ogg)   
-   [Vorbis](https://github.com/xiph/vorbis).

## Special Thanks   
[Ouroboros](https://github.com/Ouroboros), developed decompile/compile tools for Kiseki series games, and provided many
awesome hook ideas. 

------------------------------------------------------------------------

------------------------------------------------------------------------

# SoraVoice (Lite)

PC游戏《空·零·碧之轨迹》系列进化版剧情语音补丁。

您可以在[SoraVoice (Lite)](https://github.com/ZhenjianYang/SoraVoice)获取到关于本项目的全部信息。

**注意**:  本项目基于GPLv3开源协议, 对本项目的任何代码或二进制文件的复制、修改、分发需遵循此协议。
具体细节请参见[LICENSE](https://github.com/ZhenjianYang/SoraVoice/blob/master/LICENSE)文件。

### 关于 Lite

这是语音补丁[SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated)的新版本(**lite**版)。
在这个版本中，通过移除一些不常用的功能，实现了更好的兼容性以及更少的资源占用。
并且，不同于原版，lite版会在游戏更新后仍然（大概率）保持有效。

**如果您在游玩由XSeed发行的《空之轨迹》系列，请使用lite版语音补丁。**
**因为原版[SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated)已经不再维护，且无法支持最新的游戏补丁。**

## 使用方法

需要的东西:

- [游戏](#游戏)
- [语音补丁（本项目）](#语音补丁本项目)
- [语音脚本](#语音脚本)
- [语音文件](#语音文件)

### 游戏

本补丁支持从下表中的平台购买的游戏。列表以外的平台（如Wegame）可能不支持。

|游戏标题          |发行商  |语言    |平台
|------------------|--------|--------|---------------------------
|零之轨迹          |欢乐百世|简体中文|JOYO, 方块游戏
|碧之轨迹          |欢乐百世|简体中文|JOYO, 方块游戏
|空之轨迹 FC       |娱乐通  |简体中文|
|空之轨迹 SC       |娱乐通  |简体中文|
|空之轨迹 the 3RD  |娱乐通  |简体中文|
|零之轨迹          |Falcom  |日文    |
|空之轨迹 FC       |Xseed   |简体中文|Steam汉化版, [ED6-FC-Steam-CN](https://github.com/Ouroboros/ED6-FC-Steam-CN)
|空之轨迹 FC       |Xseed   |英文    |Steam/GOG/Humble
|空之轨迹 SC       |Xseed   |英文    |Steam/GOG/Humble
|空之轨迹 the 3RD  |Xseed   |英文    |Steam/GOG/Humble

### 语音补丁（本项目）

从[Release](https://github.com/ZhenjianYang/SoraVoice/releases/latest)下载最新的文件,
将压缩包内的`dinput8.dll`以及`voice`和`dll`这两个文件夹解压到游戏的安装目录即可。

#### 关于dsound.dll   
对于《空之轨迹》系列, 可以用`dsound.dll`替换掉`dinput8.dll`, 以应对`dinput8.dll`无法使用的情况(比如另一个MOD也使用了`dinput8.dll`)。
但是，请不要同时将`dsound.dll`和`dinput8.dll`置于游戏目录下。

### 语音脚本

**语音脚本**的用处是告知语音补丁每一句的对白需要播放哪一个语音文件。每一套**语音脚本**仅对应特定一个语言版本的游戏。

以下两个项目为**语音脚本**相关项目:
- [ZeroAoVoiceScripts](https://github.com/ZhenjianYang/ZeroAoVoiceScripts/releases/latest), 用于《零·碧之轨迹》。    
- [SoraVoiceScripts](https://github.com/ZhenjianYang/SoraVoiceScripts/releases/latest), 用于《空之轨迹》。

您只需在上述链接下载对应的文件，并将压缩包内的`scena`文件夹解压至`voice/`文件夹即可。

### 语音文件

我们不会提供任何的语音文件。请按照以下步骤自行生成语音文件：

#### 1. 导出游戏镜像   
您可能需要一份进化版的游戏以及一台安装了变革的PSVita来导出游戏镜像。请自行搜索相关教程。

#### 2. 解包data.psarc   
将`<进化版游戏目录>/gamedata/data.psarc`拖放至[PSArcTool](https://github.com/periander/PSArcTool)即可。   
或者，如果您有来自Sony PS3 SDK中的**psarc.exe**, 可以使用这个命令: `psarc.exe extract data.psarc`。   
   **注意**: 如果您在进化版游戏目录中找到了data0.psarc, data1.psarc, ..., 请同样解包它们。

#### 3. 转换at9文件为ogg   
  - **所需工具:**   
    - `at9tool.exe`, 来自Sony PS3 SDK   
    - [`oggenc2`](http://www.rarewares.org/ogg-oggenc.php)

1. 创建文件夹`at9`, 并将解包出的`data/talk`文件夹复制（剪切也行）到其中   
- **注意**: 对于 **《空之轨迹》系列**, 您还需复制`data/bgm/arrange/ed6501.at9`(或者`data/bgm/ed6501.at9`)到`at9/`。
(特别提醒：三作的`ed6501.at9`并不是完全相同的，请务必使用各自的`ed6501.at9`)

2. 创建包含以下内容的`Convert.bat`文件:   
~~~
@echo off
mkdir wav
mkdir ogg
for /f "delims=" %%i in ('dir /s /b /a-d at9\*.at9') do (
title converting %%~ni.at9
at9tool.exe -d -repeat 1 "%%i" "wav\%%~ni.wav"
oggenc2.exe -Q -q 6.00 -n "ogg\%%~ni.ogg" "wav\%%~ni.wav"
)
~~~
- **注意**: 参数`-q 6.00`设置了ogg的质量为6.00, 您也可以选择-2到10之间的其他值(值越高, 质量越好, 同时生成的文件的体积越大)。

3. 将`at9tool.exe`, `oggenc2.exe` and `Convert.bat`与文件夹`at9`放到一起, 然后双击运行`Convert.bat`。   
- **注意**: 这一步可能会花较长时间, 请耐心等待。   

这样, 您就可以在文件夹`ogg`中找到**语音文件**了。

#### 4. 将文件夹`ogg`复制到`voice/`下

### 运行游戏

如果所有步骤均正确完成，启动游戏后，可以在标题栏看到关于SoraVoice (Lite)的信息。

## 关于配置文件

运行游戏后，会自动生成配置文件`voice/ed_voice.ini`, 罗列配置项如下:
|                  |                                                |
|------------------|-------------------------------------------------
|ShowInfo          | 在标题栏显示关于SoraVoice (Lite)的信息
|Volume            | 音量: 0 ~ 100
|DisableTextSe     | 禁用对话框文字音效 (仅在有语音时生效)
|DisableDialogSe   | 禁用对话框关闭音效 (仅在有语音时生效) 
|DisableAoOriVoice | 禁用原始剧情语音 (仅对《碧之轨迹》生效)


### 删除的功能

相比原版语音补丁[SoraVoice](https://github.com/ZhenjianYang/SoraVoice-Deprecated), lite版删除了以下功能:
- 对话框自动前进
- 配置快捷键
- 游戏中关于语音补丁的信息提示

## 本项目使用的外部库  
-   [GoogleTest](https://github.com/google/googletest)
-   [Ogg](https://github.com/xiph/ogg)   
-   [Vorbis](https://github.com/xiph/vorbis).

## 特别感谢   
[Ouroboros](https://github.com/Ouroboros), 开发了轨迹系列游戏脚本的编译/反编译工具, 并提供了各种神奇的Hook思路。

