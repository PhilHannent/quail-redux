cmd /c "c:\Program Files (x86)\WiX Toolset v3.7\bin\candle.exe" WindowsInstallScript.wxs
cmd /c "c:\Program Files (x86)\WiX Toolset v3.7\bin\light.exe" -ext WixUiExtension WindowsInstallScript.wixobj -out Quail-installer.msi