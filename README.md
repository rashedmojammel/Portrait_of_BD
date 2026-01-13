# 🌆 Portrait of Bangladesh

A dynamic OpenGL-based simulation featuring three beautifully animated scenes showcasing the diverse landscapes of Bangladesh: a bustling Dhaka cityscape, a serene beach environment, and a peaceful rural village with a traditional boat on the river. Built with C++ and OpenGL/GLUT.

![OpenGL](https://img.shields.io/badge/OpenGL-3.3-blue)
![C++](https://img.shields.io/badge/C++-11-green)
![License](https://img.shields.io/badge/license-MIT-orange)

## ✨ Features

### 🏙️ Scene 1: Dhaka City

Experience the vibrant energy of Bangladesh's capital with this detailed urban simulation:

**Transportation System**
- Multi-layered road system with realistic traffic on three levels
- Lane-wise vehicle movement with cars, buses, and trucks in both directions
- Headlight effects and road glow during night and rain
- **Metro rail system** featuring:
  - Elevated metro line with support pillars
  - Station stop logic with acceleration/deceleration
  - Detailed train design with wheels and bogies

**City Infrastructure**
- Skyline of 14+ unique buildings including:
  - Hospital, police station, and school
  - Shopping mall and apartment complexes
  - Glass towers and eco-friendly buildings
  - Luxury hotel with distinctive architecture
- Dynamic street infrastructure with traffic lights
- Bus stop shelters and information kiosks
- Urban decorations: trees, planters, bins, and bollards

**Dynamic Lighting & Weather**
- Automatic day/night cycle (~20 seconds)
- Moon rendering with realistic positioning
- Building window lights that illuminate at night
- Street lights with ambient glow
- **Weather system** with:
  - Toggleable rain with slanted raindrops and sound effects
  - Darker storm clouds
  - Wet road reflections
  - Dimmed environment during rain

**Pedestrian Life**
- Walking people with realistic leg animation
- Automatic umbrella deployment during rain
- Pedestrians on multiple sidewalks
- Natural crowd distribution

### 🏖️ Scene 2: Beach Landscape

Immerse yourself in the tranquil beauty of Bangladesh's coastline:

**Natural Elements**
- Dynamic sky with smooth color gradients
- Separate day/night color palettes
- Layered hills creating depth and perspective
- Animated ocean with gradient effects and moving waterline

**Marine Life & Vegetation**
- Sailing boat with detailed mast, sail, and hull
- Palm trees positioned along the coast
- Tropical vegetation for authentic beach atmosphere
- Animated birds flying across the sky

**Atmospheric Effects**
- Moving clouds (primarily during daytime)
- Sun/Moon system with keyboard-controlled positioning
- Smooth day/night transitions
- Rain overlay when weather is enabled

### 🌾 Scene 3: Village & River

Discover the peaceful countryside of rural Bangladesh:

**Rural Landscape**
- Gentle sky gradient transitioning through the day
- Winding river with detailed banks
- Soil embankments and lush green fields
- Authentic rural topography

**Traditional Architecture**
- Village houses with:
  - Detailed thatched roofs
  - Doors, windows, and authentic shading
  - Traditional Bengali design elements

**Natural Environment**
- Large trees with circular foliage and detailed trunks
- Hay hills scattered across fields
- Decorative grass and wildflowers along riverbanks
- Layered vegetation for depth

**River Life**
- Traditional sailboat animation with:
  - Mast and billowing sails
  - Rowing oar with motion
  - Human character rowing the boat
- Smooth boat movement along the river
- Realistic water interaction

**Sky & Weather**
- Moving clouds across the sky (toggle control)
- Natural cloud patterns
- Atmospheric perspective

## 🎮 Controls

### General Scene Navigation
| Key | Action |
|-----|--------|
| `1` | Switch to Dhaka City scene |
| `2` | Switch to Beach scene |
| `3` | Switch to Village & Boat scene |
| `ESC` | Exit program |

### Dhaka City Scene (Scene 1)
| Key | Action |
|-----|--------|
| `N` / `n` | Switch to Night mode |
| `D` / `d` | Switch to Day mode |
| `R` / `r` | Toggle Rain ON/OFF (with sound effects) |

> **Note:** Day/night automatically cycles every ~20 seconds, adjusting clouds, lighting, and building windows.

### Beach Scene (Scene 2)
| Key | Action |
|-----|--------|
| `N` / `n` | Switch to Night mode |
| `D` / `d` | Switch to Day mode |
| `R` / `r` | Toggle Rain ON/OFF |
| `SPACE` | Pause/Resume animation |
| `+` / `=` | Increase animation speed |
| `-` / `_` | Decrease animation speed |
| `S` / `s` | Reset scene to default state |
| `Arrow Keys` | Move Sun/Moon position |
| `Left Click` | Start animation |
| `Right Click` | Pause animation |

### Village & Boat Scene (Scene 3)
| Key | Action |
|-----|--------|
| `D` | Start/continue cloud movement to the right |
| `S` | Stop cloud movement |
| `A` | Step clouds to the left (manual control) |

> **Note:** The boat and river animations run automatically via timers.

## 🛠️ Technical Requirements

### Prerequisites
- C++ compiler (GCC 7.0+ or MSVC 2017+)
- OpenGL 3.3 or higher
- GLUT (FreeGLUT recommended) or GLFW
- CMake 3.10+ (optional, for build management)



## 📦 Installation & Setup

### Clone the Repository
```bash
git clone https://github.com/yourusername/portrait-of-bangladesh.git
cd portrait-of-bangladesh
```

### Compile and Run

#### Using GCC (Linux/macOS)
```bash
g++ main.cpp -o bangladesh -lGL -lGLU -lglut
./bangladesh
```

#### Using Visual Studio (Windows)
1. Open Visual Studio and create a new C++ project
2. Add `main.cpp` to the project
3. Configure project properties:
   - Add OpenGL and GLUT include directories
   - Link against `opengl32.lib`, `glu32.lib`, and `freeglut.lib`
4. Build and run the project


### Advanced Animation System
- Timer-based animations for smooth movement
- Frame-independent animation using delta time
- Configurable animation speeds
- Pause/resume functionality

### Realistic Lighting Model
- Ambient lighting simulation
- Dynamic day/night transitions
- Object-specific lighting (street lights, windows)
- Shadow and glow effects

### Weather Simulation
- Realistic rain with angle and velocity
- Sound effects integration
- Environmental response (wet roads, umbrellas)
- Cloud density changes

### Traffic System
- Multi-lane vehicle management
- Speed variation for different vehicle types
- Collision avoidance (conceptual)
- Lane discipline implementation

## 🚀 Performance Optimization

- Efficient polygon drawing with minimal overhead
- Optimized timer callbacks for smooth animation
- Selective rendering based on scene visibility
- Memory-efficient object pooling for repetitive elements

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Contribution Ideas
- Add more scenes (tea gardens, Sundarbans mangrove forest)
- Implement water reflection shaders
- Add sound effects for vehicles and environment
- Create texture mapping for buildings
- Implement camera controls for scene exploration

## 🙏 Acknowledgments

- Inspired by the diverse landscapes and vibrant culture of Bangladesh
- Built with OpenGL and GLUT for cross-platform compatibility
- Thanks to the open-source community for libraries and resources

## 📧 Contact

**Project Maintainer:** [Rashedul alam]
- GitHub: [@yourusername](https://github.com/rashedmojammel)
- Email: rashedmojammel56@gmail.com

## 🌟 Show Your Support

If you found this project interesting, please consider giving it a ⭐️ on GitHub!

---

**Made with ❤️ to showcase the beauty of Bangladesh**
