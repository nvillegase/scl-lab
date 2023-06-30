# -*- mode: python ; coding: utf-8 -*-


block_cipher = None


a = Analysis(
    ['main.py'],
    pathex=[],
    binaries=[
        ('/home/nvillegase/Documents/un/tac/scl-project/scl-template/protocols/bgw/build/libsclbgw.so', '.'),
        ('/home/nvillegase/Documents/un/tac/scl-project/scl-template/protocols/srsq/build/libsclsrsq.so', '.'),
        ('/home/nvillegase/Documents/un/tac/scl-project/scl-template/protocols/ssnd/build/libsclssnd.so', '.'),
    ],
    datas=[
        ('/home/nvillegase/Documents/un/tac/scl-project/scl-template/protocols/parties.txt', '.'),
        ('/home/nvillegase/Documents/un/tac/scl-project/scl-template/protocols/parties_local.txt', '.'),
    ],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)
pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='main',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='main',
)
