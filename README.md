# 🎸 TremoloCuster

**Plugin de efecto de audio Tremolo** desarrollado en C++ con el framework [JUCE](https://juce.com/).

Diseñado originalmente como plugin para **GarageBand** (formato AU), también es compatible con otros DAWs mediante compilación en los formatos **VST3** y **Standalone**.

---

## 📖 ¿Qué es el Tremolo?

El tremolo es un efecto de audio que modula periódicamente el **volumen** de la señal utilizando un oscilador de baja frecuencia (LFO). Esto produce una variación rítmica de la amplitud que va desde un sutil pulso hasta un corte agresivo del sonido, ampliamente utilizado en guitarra eléctrica, órganos y sintetizadores.

---

## 🎛️ Parámetros

El plugin expone dos controles ajustables en tiempo real mediante knobs rotatorios:

| Parámetro       | ID      | Rango          | Valor por defecto | Descripción                                                      |
|-----------------|---------|----------------|--------------------|------------------------------------------------------------------|
| **Velocidad**   | `RATE`  | 0.1 – 20.0 Hz | 5.0 Hz             | Frecuencia del LFO. Controla qué tan rápido oscila el volumen.   |
| **Profundidad** | `DEPTH` | 0.0 – 1.0     | 0.8                | Intensidad del efecto. A mayor valor, mayor variación de volumen. |

---

## 🏗️ Arquitectura del Proyecto

```
TremoloCuster/
├── Source/
│   ├── PluginProcessor.h/.cpp   # Lógica DSP del efecto tremolo
│   └── PluginEditor.h/.cpp      # Interfaz gráfica (GUI) del plugin
├── JuceLibraryCode/             # Código auto-generado por JUCE (módulos y defines)
├── Builds/
│   └── MacOSX/                  # Proyecto Xcode generado por Projucer
└── TremoloCuster.jucer          # Archivo de configuración del proyecto JUCE
```

### Componentes principales

- **`PluginProcessor`** — Corazón del plugin. Implementa el procesamiento de audio (DSP):
  - Genera una onda sinusoidal (LFO) usando `std::sin()` con avance de fase por muestra.
  - Convierte la señal bipolar a unipolar para modular la ganancia.
  - Aplica la modulación de amplitud a cada muestra de cada canal de audio.
  - Los parámetros (`RATE`, `DEPTH`) se leen en tiempo real desde el `AudioProcessorValueTreeState`.

- **`PluginEditor`** — Interfaz gráfica del plugin:
  - Dos knobs rotatorios (`juce::Slider` estilo `RotaryHorizontalVerticalDrag`) para controlar velocidad y profundidad.
  - Ventana de 400×300 píxeles.
  - Conexión a los parámetros mediante `SliderAttachment` para sincronización automática con el procesador.

---

## 🔧 Requisitos

- **macOS** (el proyecto está configurado para Xcode)
- **JUCE** — Framework instalado en `/Applications/JUCE/` (o ajustar las rutas en el `.jucer`)
- **Xcode** — Para compilar el proyecto en macOS
- **Projucer** — Herramienta de JUCE para gestionar el archivo `.jucer` y regenerar los proyectos de compilación

---

## 🚀 Compilación

### Opción 1: Usando Projucer + Xcode

1. Abre `TremoloCuster.jucer` con **Projucer**.
2. Verifica que las rutas a los módulos de JUCE sean correctas (por defecto: `/Applications/JUCE/modules`).
3. Haz clic en **"Save and Open in IDE"** para abrir el proyecto en Xcode.
4. En Xcode, selecciona el target deseado:
   - **TremoloCuster - AU** → Plugin AudioUnit (compatible con GarageBand y Logic Pro)
   - **TremoloCuster - VST3** → Plugin VST3 (compatible con Ableton, Reaper, FL Studio, etc.)
   - **TremoloCuster - Standalone** → Aplicación independiente para pruebas
5. Compila con `Cmd + B` o ejecuta con `Cmd + R`.

### Opción 2: Desde la línea de comandos

```bash
# Compilar desde Xcode en línea de comandos
xcodebuild -project Builds/MacOSX/TremoloCuster.xcodeproj \
            -scheme "TremoloCuster - AU Plugin" \
            -configuration Release \
            build
```

---

## 📦 Formatos de Plugin Soportados

| Formato      | Habilitado | Uso principal                                      |
|--------------|:----------:|----------------------------------------------------|
| **AU**       | ✅          | GarageBand, Logic Pro y otros DAWs de Apple         |
| **VST3**     | ✅          | Ableton Live, Reaper, FL Studio, Cubase, etc.       |
| **Standalone** | ✅       | Ejecución independiente para pruebas y demos        |
| VST2         | ❌          | Formato legacy (deshabilitado)                      |
| AAX          | ❌          | Pro Tools (no configurado)                          |
| AUv3         | ❌          | iOS/iPadOS (no configurado)                         |
| LV2          | ❌          | Linux (no configurado)                              |

---

## 🎵 Uso en GarageBand

1. Compila el plugin en formato **AU** (AudioUnit).
2. El plugin se instala automáticamente en `~/Library/Audio/Plug-Ins/Components/`.
3. Abre **GarageBand**, crea o selecciona una pista de audio.
4. En la sección de **Plug-ins** de la pista, busca **"TremoloCuster"** dentro de la categoría de efectos del fabricante **"custer"**.
5. Ajusta los knobs de **Velocidad** y **Profundidad** a tu gusto.

> **Nota:** Si GarageBand no muestra el plugin, intenta ejecutar `auval -a` en Terminal para forzar un re-escaneo de plugins AudioUnit, o reinicia GarageBand.

---

## 🔊 Cómo funciona el DSP

El efecto tremolo se implementa con los siguientes pasos en cada bloque de audio (`processBlock`):

1. **Lectura de parámetros** — Se obtienen los valores actuales de `RATE` y `DEPTH`.
2. **Cálculo de fase** — Se calcula `phaseDelta` como `(Rate / SampleRate) × 2π`.
3. **Generación del LFO** — Para cada muestra, se calcula `sin(currentPhase)`.
4. **Conversión a unipolar** — Se transforma el rango `[-1, 1]` a `[0, 1]` con `(sin + 1) × 0.5`.
5. **Modulación de ganancia** — La ganancia final es `1.0 - (depth × LFO_unipolar)`.
6. **Aplicación** — Se multiplica cada muestra por la ganancia calculada.
7. **Avance de fase** — Se incrementa `currentPhase` y se envuelve al llegar a `2π`.

---

## 🛠️ Tecnologías

- **Lenguaje:** C++
- **Framework:** [JUCE](https://juce.com/) (módulos: audio_basics, audio_processors, gui_basics, entre otros)
- **IDE:** Xcode (macOS)
- **Gestor de proyecto:** Projucer

---

## 📄 Licencia

Este proyecto es de uso personal/educativo. Consulta los [términos de licencia de JUCE](https://juce.com/juce-7-licence/) para uso comercial del framework.

---

## 👤 Autor

**Jerry**
