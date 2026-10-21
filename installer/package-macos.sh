#!/bin/bash
set -euo pipefail
# Usage: package-macos.sh <Release artifact directory> <output directory>
source_dir="$(cd "$(dirname "$0")/.." && pwd)"
binaries="$(cd "$1" && pwd)"
mkdir -p "$2"
destination="$(cd "$2" && pwd)"
staging="$(mktemp -d)"
trap 'rm -rf "$staging"' EXIT
root="$staging/root"
mkdir -p "$root/Library/Audio/Plug-Ins/Components" "$root/Library/Audio/Plug-Ins/VST3" "$root/Applications" "$root/Library/Application Support/DUALFAT/Presets"
ditto "$binaries/AU/DUALFAT.component" "$root/Library/Audio/Plug-Ins/Components/DUALFAT.component"
ditto "$binaries/VST3/DUALFAT.vst3" "$root/Library/Audio/Plug-Ins/VST3/DUALFAT.vst3"
ditto "$binaries/Standalone/DUALFAT.app" "$root/Applications/DUALFAT.app"
cp "$source_dir"/Presets/*.dualfat "$root/Library/Application Support/DUALFAT/Presets/"
cp "$source_dir/Assets/Fonts/LICENSE-Roboto.txt" "$root/Library/Application Support/DUALFAT/"
# Disable bundle relocation: never update an old build in Downloads instead of the install target.
pkgbuild --analyze --root "$root" "$staging/components.plist"
/usr/libexec/PlistBuddy -c Print "$staging/components.plist" >/dev/null
python3 - "$staging/components.plist" <<'PY'
import plistlib,sys
p=sys.argv[1]
with open(p,'rb') as f: items=plistlib.load(f)
for item in items:
    item['BundleIsRelocatable']=False
    item['BundleHasStrictIdentifier']=True
    item['BundleOverwriteAction']='upgrade'
with open(p,'wb') as f: plistlib.dump(items,f)
PY
pkgbuild --root "$root" --component-plist "$staging/components.plist" --identifier audio.cobalt.dualfilter.installer --version 1.2.0 --install-location / --ownership recommended "$staging/payload.pkg"
architectures="$(lipo -archs "$binaries/Standalone/DUALFAT.app/Contents/MacOS/DUALFAT")"
host_arch="${architectures// /,}"
cat > "$staging/Distribution.xml" <<XML
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
 <title>DUALFAT 1.2.0</title>
 <welcome file="welcome.html" mime-type="text/html"/>
 <options customize="never" require-scripts="false" hostArchitectures="$host_arch"/>
 <domains enable_anywhere="false" enable_currentUserHome="true" enable_localSystem="false"/>
 <allowed-os-versions><os-version min="11.0"/></allowed-os-versions>
 <choices-outline><line choice="dualfat"/></choices-outline>
 <choice id="dualfat" title="DUALFAT AU, VST3 and Standalone"><pkg-ref id="audio.cobalt.dualfilter.installer"/></choice>
 <pkg-ref id="audio.cobalt.dualfilter.installer" version="1.2.0" auth="none">payload.pkg</pkg-ref>
</installer-gui-script>
XML
mkdir "$staging/resources"
cat > "$staging/resources/welcome.html" <<'HTML'
<html><body style="font-family:Helvetica,sans-serif"><h1>DUALFAT 1.2</h1><p>Dual resonant filter with independent tempo-synced LFOs.</p><p>Quit your audio host before installing. This installer updates DUALFAT for the current user, placing AU and VST3 in your Library/Audio/Plug-Ins folders and the standalone app in your Applications folder.</p><p>Your saved DAW projects and presets remain compatible. Restart your DAW and rescan plug-ins after installing.</p><p>This local build is ad-hoc signed and is not Apple-notarized.</p></body></html>
HTML
productbuild --distribution "$staging/Distribution.xml" --resources "$staging/resources" --package-path "$staging" "$destination/DUALFAT-1.2.0-macOS-${architectures// /-}.pkg"
