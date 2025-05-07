

xcopy /y/s		.\Engine\Public\*.*					.\Reference\Headers\
xcopy /y		.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy /y		.\Engine\ThirdPartyLib\*.lib		.\Reference\Librarys\
xcopy /y		.\Engine\ThirdPartyLib\*.dll		.\Client\Bin\
xcopy /y		.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy /y		.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\