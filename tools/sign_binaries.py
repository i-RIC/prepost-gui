import os
import subprocess
import sys

# ----------------------------------------------------------------------------
# Azure Key Vault code-signing credentials.
# These are read from environment variables - do NOT hardcode real values
# here. In CI (appveyor.yml) they come from the AppVeyor project's
# Environment Variables settings. For local/manual use, set them in your
# shell before running this script.
# ----------------------------------------------------------------------------
AZURE_KEY_VAULT_URI = os.environ.get("AZURE_KEY_VAULT_URI", "")
AZURE_CLIENT_ID     = os.environ.get("AZURE_CLIENT_ID", "")
AZURE_TENANT_ID     = os.environ.get("AZURE_TENANT_ID", "")
AZURE_CLIENT_SECRET = os.environ.get("AZURE_CLIENT_SECRET", "")
AZURE_CERT_NAME     = os.environ.get("AZURE_CERT_NAME", "")

TIMESTAMP_URL = "http://timestamp.globalsign.com/tsa/advanced"
DIGEST_ALGORITHM = "sha256"

REQUIRED_ENV_VARS = (
  "AZURE_KEY_VAULT_URI",
  "AZURE_CLIENT_ID",
  "AZURE_TENANT_ID",
  "AZURE_CLIENT_SECRET",
  "AZURE_CERT_NAME",
)

SIGNABLE_EXTENSIONS = (".exe", ".dll")

def check_credentials():
  missing = [name for name in REQUIRED_ENV_VARS if not os.environ.get(name)]
  if missing:
    print("error: missing required environment variable(s): " + ", ".join(missing))
    print("       set them (e.g. via the AppVeyor project's Environment Variables settings).")
    sys.exit(1)

def collect_files(paths):
  files = []
  for path in paths:
    if os.path.isdir(path):
      for root, _dirs, names in os.walk(path):
        for name in names:
          if name.lower().endswith(SIGNABLE_EXTENSIONS):
            files.append(os.path.join(root, name))
    elif os.path.isfile(path):
      files.append(path)
    else:
      print("error: path not found: " + path)
      sys.exit(1)
  return files

def main():
  paths = sys.argv[1:]
  if not paths:
    print("usage: python sign_binaries.py <file-or-dir1> [file-or-dir2 ...]")
    print("       directories are searched recursively for *.exe/*.dll")
    sys.exit(1)

  check_credentials()

  files = collect_files(paths)
  if not files:
    print("no .exe/.dll files found under: " + ", ".join(paths))
    sys.exit(0)

  cmd = [
    "AzureSignTool", "sign",
    "-kvu", AZURE_KEY_VAULT_URI,
    "-kvi", AZURE_CLIENT_ID,
    "-kvt", AZURE_TENANT_ID,
    "-kvs", AZURE_CLIENT_SECRET,
    "-kvc", AZURE_CERT_NAME,
    "-tr", TIMESTAMP_URL,
    "-td", DIGEST_ALGORITHM,
    "-fd", DIGEST_ALGORITHM,
    "-v",
    "--continue-on-error",
    "--max-degree-of-parallelism", "1",
    "--skip-signed",
  ] + files

  try:
    result = subprocess.run(cmd)
  except FileNotFoundError:
    print("error: AzureSignTool not found on PATH. Run: dotnet tool install --global AzureSignTool")
    sys.exit(1)

  sys.exit(result.returncode)

if __name__ == "__main__":
  main()
