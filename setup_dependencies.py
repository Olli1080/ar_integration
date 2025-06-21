from pathlib import Path
#from tempfile import TemporaryDirectory

from os.path import exists
from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen

#tempDir = TemporaryDirectory()

filePath = Path(__file__).parent.resolve()

if (not exists(Path.joinpath(filePath, "ar_integration.uproject"))):
    raise Exception("Not inside UE Project root or ar_integration.uproject not accessible!")

pluginPath = Path.joinpath(filePath, "Plugins", "vc-ue-extensions")

resp = urlopen("https://github.com/microsoft/vc-ue-extensions/releases/download/v2.5/VisualStudioTools_v2.5_ue54.zip")
myzip = ZipFile(BytesIO(resp.read()))
myzip.extractall(pluginPath)

#print(pluginPath)