#!/bin/sh

set -e

print_help() {
    echo "CDM-16 LLVM install script"
    echo "Usage: $0 [-d DIRECTORY] [-v RELEASE] [-u] [-s] [-h]"
    echo " -d DIRECTORY : The directory to install LLVM to. Default: ~/.llvm-cdm"
    echo " -v VERSION   : The name of the release to download. Default: nightly"
    echo " -u           : Update LLVM if it already exists."
    echo " -s           : Script mode. Don't ask interactive questions."
    echo " -h           : Display this help message."
}

get_triple() {
    os="$(uname -sm)"
    case "$os" in
        "Darwin arm64")
            echo macos-arm64
            ;;
        "Linux x86_64")
            echo linux-x86_64
            ;;
        *)
            echo "Sorry, your operating system ($os) is not supported yet." >&2
            return 1
            ;;
    esac
}

find_python() {
    python_names="python python3 python3.10 python3.11 python3.12 python3.13 python3.14"
    for python_name in $python_names; do
        if ! command -v "$python_name" >/dev/null 2>&1; then
            continue
        fi
        if ! "$python_name" -c 'import sys; assert sys.version_info >= (3, 10)' > /dev/null 2>&1; then
            continue
        fi
        echo "$python_name"
        return 0
    done
    echo "Could not find a suitable python interpreter. Please install Python 3.10 or later with your package manager." >&2
    return 1
}

check_venv() {
    if ! $1 -m venv -h >/dev/null 2>/dev/null; then
        echo "Python venv module is not installed for $1. Install it with your package manager." >&2
        return 1
    fi
}

expand_tilde() {
    case "$1" in
        "~/"* | "~")
            if [ -z "$HOME" ]; then
                echo "Can't expand '~' because \$HOME is not set." >&2
                echo "Please, re-log into the system or specify the full path explicitly" >&2
                return 1
            fi
            home_path=$1; home_path=${home_path#'~/'}; home_path=${home_path#'~'}
            echo "$HOME/$home_path"
            ;;
        *)
            echo "$1"
            ;;
    esac
}

get_default_install_dir() {
    if [ -z "$HOME" ]; then
        if [ "$1" -eq 0 ]; then
            echo "./.llvm-cdm"
            return 0
        else
            echo "Can't determine the default install directory because \$HOME is not set." >&2
            echo "Please, re-log into the system or specify the directory with -d" >&2
            return 1
        fi
    fi
    echo "$HOME/.llvm-cdm"
}

check_tools() {
    result=0
    for tool in "$@"; do
        if ! command -v "$tool" >/dev/null; then
            echo "Missing tool: $tool" >&2
            result=1
        fi
    done
    if [ "$result" -ne 0 ]; then
        echo "Please install these with your package manager." >&2
    fi
    return $result
}

check_existing() {
    if ! [ -f "$1/.cdm-llvm" ]; then
        return 1
    fi
    if ! [ -f "$1/lib/uninstall.sh" ] || ! [ -x "$1/lib/uninstall.sh" ]; then
        return 1
    fi
    . "$1/.cdm-llvm"
    if [ $# -ge 2 ]; then
        eval "$2=\"\$_version\""
    fi
}

check_install_dir() {
    if [ -f "$1" ]; then
        echo "Install directory is a file: $1" >&2
        echo "Please, specify a different install directory or remove the file." >&2
        return 1
    fi
    if ! [ -d "$1" ]; then
        eval "$2=0"
        return 0
    fi
    if check_existing "$1" _version; then
        eval "update_val=\"\$$2\""
        if [ -z "$update_val" ]; then
            echo "Directory $1 already contains LLVM version $_version"
            if ! prompt_confirm "Would you like to update the existing LLVM installation? (Y/n)"; then
                return 1
            fi
            update_val=1
            eval "$2=\"\$update_val\""
            return 0
        fi
    fi
    ls_output=$(ls -A "$1")
    if [ -n "$ls_output" ]; then
        echo "Install directory is not empty: $1" >&2
        echo "Please, specify a different install directory or remove the contents of the directory." >&2
        return 1
    fi
    eval "$2=0"
    return 0
}

prompt_input() {
    prompt_msg="$1"
    prompt_out="$2"
    prompt_cmd="$3"
    shift 3
    while true; do
        printf "%s " "$prompt_msg"
        read resp </dev/tty
        resp=$(expand_tilde "$resp") || continue
        if [ -z "$resp" ]; then
            eval "resp=\"\$$prompt_out\""
        fi
        if "$prompt_cmd" "$resp" "$@"; then
            eval "$prompt_out=\"\$resp\""
            return 0
        fi
    done
}

prompt_confirm() {
    while true; do
        printf "%s " "$1"
        read confirm_resp </dev/tty
        case "$confirm_resp" in
            [Yy]* | '')
                return 0
                ;;
            [Nn]*)
                return 1
                ;;
            *)
                echo "Please answer yes or no."
                ;;
        esac
    done
}

install_dir=
version=
triple=
no_ask=
update=
python=

while getopts "d:v:ush" opt; do
    case $opt in
        d) install_dir="$OPTARG" ;;
        v) version="$OPTARG" ;;
        u) update=1 ;;
        s) no_ask=1 ;;
        h)
            print_help
            exit 0
            ;;
        \?)
            print_help >&2
            exit 1
            ;;
    esac
done

shift $((OPTIND - 1))
if [ $# -gt 0 ]; then
    echo "$0: illegal argument: $1" >&2
    print_help >&2
    exit 1
fi

triple="$(get_triple)"
no_ask="${no_ask:-0}"
install_dir="${install_dir:-$(get_default_install_dir $no_ask)}"
version="${version:-nightly}"

if [ "$no_ask" -eq 0 ] && ! true 2>/dev/null >/dev/tty; then
    echo "Cannot run in interactive mode without a controlling terminal." >&2
    exit 1
fi

check_tools curl
python=$(find_python)
check_venv "$python"

if [ "$no_ask" -eq 0 ]; then
    prompt_input "Enter release name ($version):" version true
    prompt_input "Enter install directory ($install_dir):" install_dir check_install_dir update
    echo
else
    update="${update:-0}"
    check_install_dir "$install_dir" update
fi

echo "Host platform:            $triple"
echo "LLVM version:             $version"
echo "Install directory:        $install_dir"
if [ "$update" -ne 0 ]; then
    echo
    echo "The existing LLVM $install_dir will be uninstalled."
fi
echo
if [ "$no_ask" -eq 0 ] && ! prompt_confirm "Proceed with installation? (Y/n)"; then
    echo "Installation canceled."
    exit 0
fi

if [ "$update" -ne 0 ]; then
    echo "Uninstalling existing LLVM..."
    "$install_dir/lib/uninstall.sh" -s
fi

download_dir=$(mktemp -d "${TMPDIR:-/tmp}/llvm-cdm-download.XXXXXX")

install_success=0
cleanup() {
    rm -rf "$download_dir"
    if [ "$install_success" -eq 0 ]; then
        echo "Installation failed. Removing directory $install_dir" >&2
        rm -rf "$install_dir"
    fi
}
trap cleanup EXIT

mkdir -p "$install_dir"
install_dir_abs=$(cd "$install_dir" && pwd -P)

echo "Installing cdm-devkit..."
mkdir -p "$install_dir/lib/cdm-devkit"
$python -m venv "$install_dir/lib/cdm-devkit"
"$install_dir/lib/cdm-devkit/bin/python" -m pip install "cdm-devkit>=0.2.3"
mkdir -p "$install_dir/lib/llvm/bin"
ln -s "$install_dir_abs/lib/cdm-devkit/bin/cocas" "$install_dir/lib/llvm/bin/cocas"

download_url="https://github.com/ylab-nsu/cdm16-llvm-neo/releases/download"
echo "Downloading LLVM..."
curl --proto "=https" --tlsv1.2 -#fLo "$download_dir/llvm.tar.gz" "$download_url/$version/cdm16-llvm-$triple.tar.gz"

mkdir -p "$download_dir/llvm"
echo "Extracting LLVM..."
tar -xzf "$download_dir/llvm.tar.gz" -C "$install_dir/lib/llvm"
mkdir -p "$install_dir/bin"
ln -s "$install_dir_abs/lib/llvm/bin/clang" "$install_dir/bin/clang-cdm"

llvm_info="$install_dir/.cdm-llvm"
uninstall_script="$install_dir/lib/uninstall.sh"

echo "Writing LLVM info..."
cat >"$llvm_info" <<EOF
_version="$version"
EOF

echo "Writing uninstall script..."
cat >"$uninstall_script" <<EOF
#!/bin/sh

set -e

print_help() {
    echo "CDM-16 LLVM uninstall script"
    echo "Usage: \$0 [-s] [-h]"
    echo " -s : Script mode. Don't ask interactive questions."
    echo " -h : Display this help message."
}

prompt_confirm() {
    while true; do
        printf "%s " "\$1"
        read resp </dev/tty
        case "\$resp" in
            [Yy]* | '')
                return 0
                ;;
            [Nn]*)
                return 1
                ;;
            *)
                echo "Please answer yes or no."
                ;;
        esac
    done
}

install_dir="$install_dir_abs"
no_ask=0

while getopts "sh" opt; do
    case \$opt in
        s) no_ask=1 ;;
        h)
            print_help
            exit 0
            ;;
        \?)
            print_help >&2
            exit 1
            ;;
    esac
done

shift \$((OPTIND - 1))
if [ \$# -gt 0 ]; then
    echo "\$0: illegal argument: \$1" >&2
    print_help >&2
    exit 1
fi

if [ "\$no_ask" -eq 0 ] && ! true 2>/dev/null >/dev/tty; then
    echo "Cannot run in interactive mode without a controlling terminal." >&2
    exit 1
fi

echo "Directory \$install_dir will be DELETED."
echo
if [ "\$no_ask" -eq 0 ] && ! prompt_confirm "Proceed with uninstallation? (Y/n)"; then
    echo "Uninstallation canceled."
    exit 0
fi

uninstall_success=0
on_exit(){
    if [ "\$uninstall_success" -eq 0 ]; then
        echo "Uninstallation failed."
    fi
}
trap on_exit EXIT

echo "Removing LLVM files..."
cd /
rm -rf "\$install_dir"

echo "Uninstallation completed successfully."
uninstall_success=1
EOF
chmod a+x "$uninstall_script"
echo "Uninstall script written to $uninstall_script"
echo "Installation completed successfully."
if ! command -v "clang-cdm" >/dev/null 2>&1; then
    echo
    echo "To make 'clang-cdm' command available anywhere, add $install_dir_abs/bin to your PATH."
    echo
    echo "bash: add this line to ~/.bash_profile or ~/.bashrc"
    echo "  export PATH=\"$install_dir_abs/bin:\$PATH\""
    echo
    echo "zsh: add this line to ~/.zprofile or ~/.zshrc"
    echo "  export PATH=\"$install_dir_abs/bin:\$PATH\""
    echo
    echo "fish: run this line once or add it to ~/.config/fish/config.fish"
    echo "  fish_add_path \"$install_dir_abs/bin\""
    echo
    echo "Then restart the shell or source the config file."
fi
install_success=1
