cmd /c "%WIX%\bin\candle.exe" WindowsInstallScript.wxs
cmd /c "%WIX%\bin\light.exe" -ext WixUiExtension WindowsInstallScript.wixobj -out Quail-installer.msi