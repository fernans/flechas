# flechas (head/face tracking prototypes)

> **This project is shared ONLY for educational purposes. Its use, direct or derived, for professional or economic purposes is not permitted.**

A collection of early C++ head- and face-tracking prototypes built on Seeing Machines faceAPI, together with PittPatt FTR-SDK reference material and a Qt-based image-push sample. The code prints head-pose, face and landmark data from a live camera and was used to explore cursor/arrow-style interaction driven by head movement.

For security reasons, some parts of the project may have been edited or removed. See "Excluded and preserved content" below.

## Requirements / How to run

- Windows (the samples target the XP/Vista-era Visual C++ toolchain; `.vcproj` and `.cbp` project files are included).
- Seeing Machines faceAPI SDK headers/libraries/runtime for the console samples; `dev/flechas/TestAppConsole.cpp` and `test1/Console/TestAppConsole.cpp` demonstrate head-pose callbacks and direct polling.
- Build `dev/flechas/TestAppConsole.vcproj` (or `test1/Console/TestAppConsole.vcproj`) and run the resulting executable with a webcam connected. Keys: `r` restarts tracking, `l` toggles lip tracking, `e` toggles eyebrow tracking, `h` toggles head-pose printing, `q` quits.
- `test1/ImagePush/` is a Qt/C++ sample that pushes raw frames into faceAPI. Comment out `USE_PS3CAMERA` in `stdafx.h` to use a standard webcam, or set `SM_API_PS3EYELIB` to the PS3 Eye driver install location as described in `test1/ImagePush/ReadMe.txt`.
- Third-party SDK documentation and sample data are kept under `docs/faceapi/` and `docs/pittpatt/`. Prebuilt executables are included where provided; no package manager or automated test suite is configured.

## Large assets

> **Large files / multimedia assets**: files larger than 100 MB are not stored in this repository due to size. Download `flechas-assets.zip` from the **Releases** tab and extract it into the project root, preserving the folder structure.

The ZIP contains the five excluded archives/installers: `docs/pittpatt/pittpatt_ftr_sdk-4.1-windows-x86.zip`, `docs/pittpatt/pittpatt_ftr_sdk-4.1-windows-x86_64.zip`, `docs/pittpatt/pittpatt_ftr_sdk-4.1-mac_os_x-x86_64.tgz`, `docs/faceapi/FaceTrackingAPI_NC-V3.2.0.zip` and `docs/faceapi/FaceTrackingAPI_NC-V3.2.0/setup_FaceTrackingAPI_NC-V3.2.0.exe`.

## Excluded and preserved content

- `docs/pittpatt/pittpatt_license.h` contained an expired third-party SDK license ID/key and is excluded from publication. No keygens, cracks, private keys, certificates or hard-coded passwords were found in the publishable files.
- Public URLs, third-party support emails and `C:\Program Files\...` paths in vendor documentation were intentionally preserved, as were third-party copyright and license headers.
- Visual Studio intermediate build folders (`Debug Win32/`, `Release Win32/`), IDE `.depend`/`.layout` files and `.DS_Store` are ignored; source, project files, prebuilt executables and smaller SDK files are kept.

## Disclaimer

The software is provided "as is", without warranty of any kind. No liability is accepted for any errors or damages resulting from its use.

> **AI training**: the use of the content of this repository (code, assets, documentation) to train or fine-tune machine learning models or AI systems is expressly **not permitted**. All of the repository is licensed under CC-BY-NC-4.0 (see the `LICENSE` file).

## License

Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0). See `LICENSE`.

---

# flechas (prototipos de seguimiento de cabeza/rostro)

> **Este proyecto se comparte SOLO con valor educativo. No está permitido su uso (directo o derivado) para fines profesionales o económicos.**

Una colección de prototipos tempranos en C++ de seguimiento de cabeza y rostro basados en Seeing Machines faceAPI, junto con material de referencia del SDK PittPatt FTR y una muestra Qt de inserción de imágenes. El código muestra datos de pose de cabeza, rostro y puntos de referencia de una cámara en vivo y se empleó para explorar una interacción tipo cursor/flecha controlada por el movimiento de la cabeza.

Por motivos de seguridad, es posible que algunas partes se encuentren editadas o eliminadas. Consulta «Contenido excluido y conservado» más abajo.

## Requisitos / Cómo ejecutar

- Windows (las muestras están pensadas para la cadena de herramientas de Visual C++ de la época de XP/Vista; se incluyen archivos de proyecto `.vcproj` y `.cbp`).
- Cabeceras, bibliotecas y runtime del SDK Seeing Machines faceAPI para las muestras de consola; `dev/flechas/TestAppConsole.cpp` y `test1/Console/TestAppConsole.cpp` muestran callbacks de pose de cabeza y lectura directa.
- Compila `dev/flechas/TestAppConsole.vcproj` (o `test1/Console/TestAppConsole.vcproj`) y ejecuta el resultado con una webcam conectada. Teclas: `r` reinicia el seguimiento, `l` alterna el seguimiento de labios, `e` alterna el seguimiento de cejas, `h` alterna la impresión de pose de cabeza y `q` sale.
- `test1/ImagePush/` es una muestra Qt/C++ que envía fotogramas crudos a faceAPI. Comenta `USE_PS3CAMERA` en `stdafx.h` para usar una webcam estándar, o define `SM_API_PS3EYELIB` con la ruta de instalación del controlador PS3 Eye, como se indica en `test1/ImagePush/ReadMe.txt`.
- La documentación y los datos de muestra de SDK de terceros están en `docs/faceapi/` y `docs/pittpatt/`. Se conservan los ejecutables precompilados cuando se aportaron; no hay gestor de paquetes ni suite de pruebas automatizadas.

## Archivos grandes

> **Archivos grandes / recursos multimedia**: los archivos de más de 100 MB no se incluyen en este repositorio por su tamaño. Descarga `flechas-assets.zip` desde la pestaña **Releases** y extráelo en la raíz del proyecto, manteniendo la estructura de carpetas.

El ZIP contiene los cinco archivos excluidos: `docs/pittpatt/pittpatt_ftr_sdk-4.1-windows-x86.zip`, `docs/pittpatt/pittpatt_ftr_sdk-4.1-windows-x86_64.zip`, `docs/pittpatt/pittpatt_ftr_sdk-4.1-mac_os_x-x86_64.tgz`, `docs/faceapi/FaceTrackingAPI_NC-V3.2.0.zip` y `docs/faceapi/FaceTrackingAPI_NC-V3.2.0/setup_FaceTrackingAPI_NC-V3.2.0.exe`.

## Contenido excluido y conservado

- `docs/pittpatt/pittpatt_license.h` contenía un identificador/clave de licencia caducada de un SDK de terceros y se excluye de la publicación. No se encontraron keygens, cracks, claves privadas, certificados ni contraseñas evidentes en los archivos que se publican.
- Las URLs públicas, los correos de soporte de terceros y las rutas `C:\Program Files\...` de la documentación de proveedores se conservaron deliberadamente, al igual que las cabeceras de copyright y licencia de terceros.
- Las carpetas intermedias de Visual Studio (`Debug Win32/`, `Release Win32/`), los archivos `.depend`/`.layout` del IDE y `.DS_Store` se ignoran; se conservan el código, los archivos de proyecto, los ejecutables precompilados y los archivos más pequeños del SDK.

## Aviso

El software se proporciona «tal cual», sin ningún tipo de garantía. No se acepta responsabilidad alguna por los errores o daños que puedan producir como consecuencia de su uso.

> **Entrenamiento de IA**: no se permite expresamente el uso del contenido de este repositorio (código, recursos, documentación) para entrenar o ajustar (*fine-tune*) modelos de aprendizaje automático o sistemas de IA. Todo el repositorio se publica con licencia CC-BY-NC-4.0 (ver el archivo `LICENSE`).

## Licencia

Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0). Consulta `LICENSE`.
