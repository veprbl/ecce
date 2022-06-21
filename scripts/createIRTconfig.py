# produce auxiliary ROOT file for the IRT algorithm
import xml.etree.ElementTree as et
import shutil, os, DDG4

def run():

    # compact files
    mainFile  = 'ecce.xml'
    richFile = 'compact/drich.xml'

    # backup the RICH compact file
    shutil.copy(richFile,richFile+'.bak')

    # parse the RICH compact file, enable `DRICH_create_irt_file` mode, then overwrite
    compactTree = et.parse(richFile)
    for constant in compactTree.iter(tag='constant'):
        if(constant.attrib['name']=='DRICH_create_irt_file'):
            constant.set('value','1')
    compactTree.write(richFile)

    # produce IRT config file
    try:
        kernel = DDG4.Kernel()
        kernel.loadGeometry(str(f'file:{mainFile}'))
        kernel.terminate()
    except:
        pass

    # revert to the original compact file
    os.replace(richFile+'.bak',richFile)

if __name__ == "__main__":
    run()
