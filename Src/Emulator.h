#include <1stParty/OpenGL_Loader/Include/GLES3/gl3_loader.h>
#include <VrAppFramework/Include/OVR_Input.h>
#include <gearboy.h>
#include <string>

namespace Emulator {

using namespace OVR;

struct Rom {
  bool isGbc;
  std::string RomName;
  std::string FullPath;
  std::string FullPathNorm;
  std::string SavePath;
};

struct SaveState {
  bool filled;
  GB_Color *saveImage;
};

struct LoadedGame {
  SaveState saveStates[10];
};

#define VIDEO_WIDTH 160
#define VIDEO_HEIGHT 144

extern LoadedGame *currentGame;

extern GLuint textureID, stateImageId;

extern const int CylinderWidth, CylinderHeight;

extern ovrTextureSwapChain *CylinderSwapChain;

extern int selectedPalette;

extern int button_mapping_index[2];

extern bool forceDMG;

void Init(std::string stateFolder);

void Delete();

void InitScreen();

void InitStateImage();

void UpdateStateImage(int saveSlot);

void ChangePalette(int dir);

void ResetGame();

void ChangeButtonMapping(int buttonIndex, int dir);

void SaveSettings(std::ofstream *outfile);

void LoadSettings(std::ifstream *file);

void LoadGame(Rom *rom);

void SaveRam();

void LoadRam();

void SaveStateImage(int slot);

bool LoadStateImage(int slot);

void SaveState(int saveSlot);

void LoadState(int slot);

void Update(const ovrFrameInput &vrFrame, unsigned int lastButtonState);

void SetPalette(GB_Color *newPalette);

bool SortByRomName(const Rom &first, const Rom &second);

}  // namespace Emulator