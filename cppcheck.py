import os
import subprocess
import sys


print("\n---------CPPCHECK----------")

if not os.path.exists('build-cppcheck'):
    os.mkdir('build-cppcheck')

cppcheckCommand = [
    'cppcheck',
    '--cppcheck-build-dir=build-cppcheck',
    '--error-exitcode=1',
    '--enable=all',
    '--suppressions-list=.suppress.cppcheck',
    '--inline-suppr',
    '--std=c++20',
    '--language=c++'
]

startDirectory = sys.argv[1]

additionalArgs = sys.argv[2:]
cppcheckCommand.extend(additionalArgs)

filesToCheck = []
for root, _, files in os.walk(startDirectory):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.hpp') or file.endswith('.h'):
            filesToCheck.append(os.path.join(root, file))


cppcheckCommand.extend(filesToCheck)
result = subprocess.run(cppcheckCommand)
if result.returncode != 0:
    print(f"Cppcheck failed with exit code {result.returncode}")
    exit(result.returncode)

print("-------CPPCHECK DONE----------")
exit(result.returncode)