import json
import uuid
import sys
import os
from enum import Enum
from pathlib import Path
import yaml

"""
TODO: add a check to ensure only unique asset names exist!

"""

ASSETS_BASE_PATH: str = "resources/assets/"
ASSET_DB_PATH: str = f"{ASSETS_BASE_PATH}asset-db.txt"
ASSET_ROOT_PATH_FROM_EXE: str = "../"

METADATA_FILE: str = ".metadata.txt"
ASSET_DB_HPP_PATH: str = "src/AssetLoader/src/"

def write_file(path: str, data: str) -> bool:
    try:
        with open(path, "w") as file:
            if isinstance(data, list):
                for item in data:
                    file.write(str(item) + "\n")
            else:
                file.write(str(data))
        return True
    except Exception as e:
        print(f"Exception occured when writing to file: {path}. Error: {e}")
        return False
    
def update_timestamp_metadata(modifiedDirs) -> None:
    for dir in modifiedDirs:
        metaDataContent = []
        dirFiles = [
            file for file in os.listdir(dir)
            if os.path.isfile(os.path.join(dir, file)) and file != METADATA_FILE
        ]
        dirPath = dir + "/"
        for file in dirFiles:
            metaDataContent.append(file)
            metaDataContent.append(os.path.getmtime(dirPath + file))

        metaDataFilePath = dirPath + METADATA_FILE
        result = write_file(metaDataFilePath, metaDataContent)
        if result:
            print(f"[INFO] Updating timestamp in file: {metaDataFilePath} was successfull")
        else:
            print(f"[ERROR] Updating timestamp in file: {metaDataFilePath} failed")

def is_dir_modified(dirPath: str) -> bool:
    metadataFile: str = dirPath + "/" + METADATA_FILE
    metaDataContent = read_file(metadataFile)
    if metaDataContent == None:
        return True
    
    metaDataContent = metaDataContent.splitlines()
    dirFiles = [
            file for file in os.listdir(dirPath)
            if os.path.isfile(os.path.join(dirPath, file)) and file != METADATA_FILE
        ]
    
    if len(metaDataContent) < 1 and len(dirFiles) < 1:
        return False
    
    for i in range(0, len(metaDataContent), 2):
        metadataFile: str = metaDataContent[i]
        if metadataFile not in dirFiles:
            return True

    TIMESTAMP_OFFSET: int = 1
    for file in dirFiles:
        if file not in metaDataContent:
            return True
        else:
            index = metaDataContent.index(file)
            fullFileName = dirPath + "/" + file

            storedTimeStamp = metaDataContent[index + TIMESTAMP_OFFSET]
            actualTimeStamp = os.path.getmtime(fullFileName)
            if float(storedTimeStamp) != float(actualTimeStamp):
                return True
        
    return False

def read_dir_files(path: str):
    return os.listdir(path)

def read_json_file(path: str):
    if not os.path.exists(path):
        return None
    try:
        with open(path, "r") as file:
            content: dict = json.load(file)
            return content
    except Exception as e:
        
        print(e)
        return None

def read_file(path: str):
    if not os.path.exists(path):
        return None
    try:
        with open(path, "r") as file:
            content: dict = file.read()
            return content
    except Exception as e:
        print(e)
        return None

def move_numbers_to_end(string: str):
    i = 0
    while i < len(string) and string[i].isdigit():
        i+=1
    return string[i:] + string[:i]

def build_cpp_code(fileType: str) -> list[str]:
    content: list[str] = []
    NAME_INDEX: int = 0
    UUID_OFFSET: int = 1

    for i in range(1, len(assetDB), 2):
        UUID = assetDB[i - UUID_OFFSET]
        asset = assetDB[i]
        if asset.endswith(fileType):
            assetName = os.path.basename(asset).split(".")[NAME_INDEX]
            if assetName[0].isdigit():
                assetName = move_numbers_to_end(assetName)
            #uuidAsByteArray = ", ".join(str(byte) for byte in uuid.UUID(UUID).bytes)
            uuidAsHexArray = ", ".join(f"0x{byte:02x}" for byte in uuid.UUID(UUID).bytes)
            content.append(f"\tstatic constexpr UUID {assetName} {{std::array<std::uint8_t, 16> {{{uuidAsHexArray}}} }};")
    return content

def generate_hpp() -> list[str]:
    hppFileContent: list[str] = []
    hppFileContent.append("#pragma once")
    hppFileContent.append('#include "UUID.hpp"\n')

    hppFileContent.append("namespace asl {")

    hppFileContent.append("namespace Models {")
    hppFileContent.extend(build_cpp_code(".glb"))
    hppFileContent.append("} // Models")

    hppFileContent.append("namespace Sounds {")
    hppFileContent.extend(build_cpp_code(".wav"))
    hppFileContent.append("} // Sounds")

    hppFileContent.append("} // asl")

    return hppFileContent

def remove_files(files: list[Path]):
    for file in files:
        file.unlink()

def file_in_dir(dir, file):
    for f in dir:
        if file in f:
            return f
        
def clean_metadata_files():
    metadataFiles = []
    for path in Path(ASSETS_BASE_PATH).rglob('*'):
        if path.is_file() and path.name == METADATA_FILE:
            metadataFiles.append(path)

    if metadataFiles:
        print("[INFO] Found metadatafiles. Will delete them.")
        remove_files(metadataFiles)
    else:
        print("[INFO] Found no metadatafiles.")

if __name__ == "__main__":
    if not os.path.exists(ASSET_DB_PATH):
        print("[INFO] No database was found, will create it")
        assetDBContent = ''
        writeResult = write_file(ASSET_DB_PATH, assetDBContent)
        clean_metadata_files()
    elif len(read_file(ASSET_DB_PATH)) < 1:
        clean_metadata_files()
        
    modifiedDirs: list[str] = []
    allCurrentFiles: list[str] = []
    for path in Path(ASSETS_BASE_PATH).rglob('*'):
        if path.is_dir():
            for file in path.iterdir():
                if file.is_file() and file.name != METADATA_FILE:
                    allCurrentFiles.append(file.as_posix())
            if is_dir_modified(path.as_posix()):
                modifiedDirs.append(path.as_posix())
                      
    if not modifiedDirs:
        print("[INFO] Asset directories have not been modified.")
        sys.exit(0)
    
    print("[INFO] Asset directories that was found to be modified: ", modifiedDirs)
    print()

    assetDB = read_file(ASSET_DB_PATH).splitlines()
    if assetDB == None:
       print(f"[ERROR] Could not read file {ASSET_DB_PATH}")
       sys.exit(1)
    
    for file in allCurrentFiles:
        if file not in assetDB:
            print(f"[INFO] file does not exists in asset-DB: {file}")
            newUUID = uuid.uuid4()
            assetDB.append(newUUID.hex)
            assetDB.append(file)

    # ISSUE: If we dont retrieve all asset files that is available, not just those that is contained in modifiedDirs.
    # We will automatically remove those from the database....
    UUID_OFFSET: int = 1
    for asset in assetDB[1::2].copy():
        if asset not in allCurrentFiles:
            print(f"[INFO] Asset does not exists in directories: {asset}")
            UUID = assetDB[assetDB.index(asset) - UUID_OFFSET]
            assetDB.remove(asset)
            assetDB.remove(UUID)

    writeResult = write_file(ASSET_DB_PATH, assetDB)
    if writeResult:
        update_timestamp_metadata(modifiedDirs)
    else:
        print("[ERROR] Writing updated database to disk failed")
        sys.exit(1)

    hppCode = generate_hpp()
    writeResult: bool = write_file(f"{ASSET_DB_HPP_PATH}UUIDDefines.hpp", hppCode)
    if writeResult:
        print("[INFO] Writing generated .hpp code from asset database was successfull.")
    else:
        print("[ERROR] Writing generated .hpp code from asset database failed")
