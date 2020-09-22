/**
*
*                      PulseAim
*			 github.com/ytmcgamer/PulseAim
*                 made by YTMcGamer#1337
*
*/
#include "../Incl.h"
#include "../Header/Config.h"

//Settings Settings = { 0 };

namespace SettingsHelper {
	VOID SaveSettings() {
		CHAR path[0xFF];
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("fnambt.settings"));

		auto file = fopen(path, ("wb"));
		if (file) {
			fwrite(&Settings, sizeof(Settings), 1, file);
			fclose(file);
		}
	}

	VOID ResetSettings() {
		Settings = { 0 };
		Settings.Page = 1;
		Settings.AimPoint = 0;
		Settings.Aimbot = false;//
		Settings.SilentAimbot = false;
		Settings.FlickAimbot = false;
		Settings.SpinBot = false;
		Settings.AutoAim = false;
		Settings.SpamAutoAim = false;
		Settings.TriggerAimbot = false;
		Settings.TriggerSpeed = 0;
		Settings.AutoAimbot = false;
		Settings.NoSpreadAimbot = false;
		Settings.RapidFire = false;
		Settings.BulletTP = false;
		Settings.ARTP = false;
		Settings.BoatRocketTP = false;
		Settings.RocketTP = false;
		Settings.BetterMap = false;
		Settings.ThirdPerson = false;//
		Settings.AirStuck = false;
		Settings.AntiAim = false;
		Settings.FreeCamRotationLock = false;
		Settings.FreeCam = false;
		Settings.FreeCamSpeed = 1.00f;
		Settings.CheckVisible = false;//
		Settings.AimbotFOV = 200.0f;
		Settings.AimbotSlow = 0.0f;
		Settings.InstantReload = false;
		Settings.StreamSnipe = false;
		Settings.FOVSlider = false;//
		Settings.FOV = 100.0f;
		Settings.DrawAimbotFOV = true;
		Settings.DrawFilledAimbotFOV = false;//
		Settings.CrosshairSize = 0.0f;
		Settings.CrosshairThickness = 0.0f;
		Settings.AimbotFOV = true;//  ********************
		Settings.Players = false;//
		Settings.PlayerBox = false;//
		Settings.PlayersCorner = false;
		Settings.PlayerLines = true;//
		Settings.ClosestLineESP = false;//
		Settings.PlayerLinesLocation = 1;
		Settings.PlayerNames = false;//
		Settings.BoxESPOpacity = 0.20f;
		Settings.FOVCircleOpacity = 1.00f;
		Settings.FOVCircleFilledOpacity = 0.20f;
		Settings.PlayerVisibleColor[0] = 0.0f;
		Settings.PlayerVisibleColor[1] = 0.0f;
		Settings.PlayerVisibleColor[2] = 0.0f;
		Settings.PlayerNotVisibleColor[0] = 0.0f;
		Settings.PlayerNotVisibleColor[1] = 0.0f;
		Settings.PlayerNotVisibleColor[2] = 0.0f;
		Settings.PlayerTeammate[0] = 1.0f;
		Settings.PlayerTeammate[1] = 0.00f;
		Settings.PlayerTeammate[2] = 1.0f;
		Settings.FOVCircleColor[0] = 0.0f;
		Settings.FOVCircleColor[1] = 0.20f;
		Settings.FOVCircleColor[2] = 0.0f;
		Settings.BoxESP[0] = 0.0f;
		Settings.BoxESP[1] = 0.0f;
		Settings.BoxESP[2] = 0.0f;
		Settings.LineESP[0] = 0.46f;
		Settings.LineESP[1] = 0.0f;
		Settings.LineESP[2] = 0.0f;
		Settings.Ammo = false;
		Settings.Chest = false;
		Settings.Llama = true;
		Settings.boat = true;
		Settings.chopper = true;
		Settings.Extra = true;

		// keybinds
		Settings.keybind.Menu = 0x91;
		Settings.keybind.AimbotLock = 0x02;
		Settings.keybind.AimbotShoot = 0x01;
		Settings.keybind.AntiAim = 0x01;
		Settings.keybind.Spinbot = 0x14;
		Settings.keybind.Freecam = 0;
		Settings.keybind.StreamSnipe = 0;
		Settings.keybind.ThirdPerson = 0;
		Settings.keybind.Airstuck;
		Settings.keybind.Airstuck2 = 0;

		SaveSettings();
	}

	VOID Initialize() {
		CHAR path[0xFF] = { 0 };
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("gayfaggot.settings"));

		auto file = fopen(path, ("rb"));
		if (file) {
			fseek(file, 0, SEEK_END);
			auto size = ftell(file);

			if (size == sizeof(Settings)) {
				fseek(file, 0, SEEK_SET);
				fread(&Settings, sizeof(Settings), 1, file);
				fclose(file);
			}
			else {
				fclose(file);
				ResetSettings();
			}
		}
		else {
			ResetSettings();
		}
	}
}