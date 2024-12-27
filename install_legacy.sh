#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

BIN_DIR="/usr/bin"
# Target directory for the libraries
LIB_DIR="/usr/lib"
INC_DIR="/usr/include/compra"

# Directory containing the .so files 
LIB_SOURCE_DIR="./build/lib"

# Directory containing the binaries
BIN_SOURCE_DIR="./build/bin"

# Directory containing the header files
INC_SOURCE_DIR="./include/compra"

# Define the list of files to copy
LIBS=("libcompra.so")
BINS=()
HEADERS=("compra.h" "export.h")

# Variable to handle automatic "yes" answer
AUTO_YES=false

# Function to check for root privileges
check_root() {
  if [ "$EUID" -ne 0 ]; then
    echo "Please run this script with sudo or as root."
    exit 1
  fi
}

# Function to install libraries
install_libraries() {
  echo "The following library files will be installed to $LIB_DIR:"
  for FILE in "${LIBS[@]}"; do
    echo "$LIB_SOURCE_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    # Confirm installation
    read -p "Do you want to proceed with installing libraries? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  # Copy the libraries
  echo "Copying library files to $LIB_DIR..."
  for FILE in "${LIBS[@]}"; do
    if [ -f "$LIB_SOURCE_DIR/$FILE" ]; then
      cp "$LIB_SOURCE_DIR/$FILE" "$LIB_DIR"
    else
      echo "Warning: $LIB_SOURCE_DIR/$FILE not found. Skipping."
    fi
  done

  # Update the linker cache
  echo "Updating the dynamic linker cache..."
  ldconfig

  echo "Library installation complete!"
}

# Function to install binaries
install_binaries() {
  echo "The following binary files will be installed to $BIN_DIR:"
  for FILE in "${BINS[@]}"; do
    echo "$BIN_SOURCE_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    # Confirm installation
    read -p "Do you want to proceed with installing binaries? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  # Copy the binaries
  echo "Copying binary files to $BIN_DIR..."
  for FILE in "${BINS[@]}"; do
    if [ -f "$BIN_SOURCE_DIR/$FILE" ]; then
      cp "$BIN_SOURCE_DIR/$FILE" "$BIN_DIR"
    else
      echo "Warning: $BIN_SOURCE_DIR/$FILE not found. Skipping."
    fi
  done

  echo "Binary installation complete!"
}

# Function to install header files
install_headers() {
  mkdir -p "$INC_DIR"
  echo "The following header files will be installed to $INC_DIR:"
  for FILE in "${HEADERS[@]}"; do
    echo "$INC_SOURCE_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    # Confirm installation
    read -p "Do you want to proceed with installing header files? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  # Copy the header files
  echo "Copying header files to $INC_DIR..."
  for FILE in "${HEADERS[@]}"; do
    if [ -f "$INC_SOURCE_DIR/$FILE" ]; then
      cp "$INC_SOURCE_DIR/$FILE" "$INC_DIR"
    else
      echo "Warning: $INC_SOURCE_DIR/$FILE not found. Skipping."
    fi
  done

  echo "Header file installation complete!"
}

# Function to uninstall libraries
uninstall_libraries() {
  echo "The following library files will be removed from $LIB_DIR:"
  for FILE in "${LIBS[@]}"; do
    echo "$LIB_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    # Confirm uninstallation
    read -p "Do you want to proceed with uninstalling libraries? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  # Remove the libraries
  echo "Removing library files from $LIB_DIR..."
  for FILE in "${LIBS[@]}"; do
    if [ -f "$LIB_DIR/$FILE" ]; then
      rm "$LIB_DIR/$FILE"
      echo "Removed $LIB_DIR/$FILE"
    else
      echo "Warning: $LIB_DIR/$FILE not found. Skipping."
    fi
  done

  # Update the linker cache
  echo "Updating the dynamic linker cache..."
  ldconfig

  echo "Library uninstallation complete!"
}

# Function to uninstall binaries
uninstall_binaries() {
  echo "The following binary files will be removed from $BIN_DIR:"
  for FILE in "${BINS[@]}"; do
    echo "$BIN_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    # Confirm uninstallation
    read -p "Do you want to proceed with uninstalling binaries? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  # Remove the binaries
  echo "Removing binary files from $BIN_DIR..."
  for FILE in "${BINS[@]}"; do
    if [ -f "$BIN_DIR/$FILE" ]; then
      rm "$BIN_DIR/$FILE"
      echo "Removed $BIN_DIR/$FILE"
    else
      echo "Warning: $BIN_DIR/$FILE not found. Skipping."
    fi
  done

  echo "Binary uninstallation complete!"
}

# Function to uninstall header files
uninstall_headers() {
  echo "The following header files will be removed from $INC_DIR:"
  for FILE in "${HEADERS[@]}"; do
    echo "$INC_DIR/$FILE"
  done
  echo

  if ! $AUTO_YES; then
    read -p "Do you want to proceed with uninstalling header files? (y/n): " CONFIRM
    if [[ "$CONFIRM" != "y" ]]; then
      echo "Operation canceled."
      exit 0
    fi
  fi

  echo "Removing header files from $INC_DIR..."
  for FILE in "${HEADERS[@]}"; do
    if [ -f "$INC_DIR/$FILE" ]; then
      rm "$INC_DIR/*"
    fi
  done

  rm -rf "$INC_DIR"
  echo "Headers uninstallation complete!"
}
 
# Function to reinstall libraries and binaries
reinstall_libraries_binaries_and_headers() {
  uninstall_libraries
  uninstall_binaries
  uninstall_headers
  install_libraries
  install_binaries
  install_headers
}

# Parse arguments for -y flag anywhere in the command line
for arg in "$@"; do
  if [ "$arg" == "-y" ]; then
    AUTO_YES=true
    break
  fi
done

# Main script logic
case "$1" in
  install)
    check_root
    install_libraries
    install_binaries
    install_headers
    ;;
  uninstall)
    check_root
    uninstall_libraries
    uninstall_binaries
    uninstall_headers
    ;;
  reinstall)
    check_root
    reinstall_libraries_binaries_and_headers
    ;;
  *)
    echo "Usage: $0 {install|uninstall|reinstall} [-y]"
    exit 1
    ;;
esac
