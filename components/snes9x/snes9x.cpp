/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#include <ctype.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "snes9x.h"
#include "memmap.h"
#include "controls.h"
#include "display.h"
#include "conffile.h"
#ifdef NETPLAY_SUPPORT
#include "netplay.h"
#endif

#ifdef DEBUGGER
#include "debug.h"
extern FILE	*trace;
#endif

#define S9X_CONF_FILE_NAME	"snes9x.conf"

static char	*rom_filename = NULL;

static bool parse_controller_spec (int, const char *);
static bool try_load_config_file (const char *, ConfigFile &);

void time_trace(const char *file, int line)
{
	static uint32_t timer;
	uint32_t time = (esp_timer_get_time() / 1000);

	if ((time - timer) >= 1000) {
		printf("Time trace %s:%d %.2fms\n", file, line, (float)(time - timer));
		timer = time;
	}
}

static bool parse_controller_spec (int port, const char *arg)
{
	if (!strcasecmp(arg, "none"))
		S9xSetController(port, CTL_NONE,       0, 0, 0, 0);
	else
	if (!strncasecmp(arg, "pad",   3) && arg[3] >= '1' && arg[3] <= '8' && arg[4] == '\0')
		S9xSetController(port, CTL_JOYPAD, arg[3] - '1', 0, 0, 0);
	else
	if (!strncasecmp(arg, "mouse", 5) && arg[5] >= '1' && arg[5] <= '2' && arg[6] == '\0')
		S9xSetController(port, CTL_MOUSE,  arg[5] - '1', 0, 0, 0);
	else
	if (!strcasecmp(arg, "superscope"))
		S9xSetController(port, CTL_SUPERSCOPE, 0, 0, 0, 0);
	else
	if (!strcasecmp(arg, "justifier"))
		S9xSetController(port, CTL_JUSTIFIER,  0, 0, 0, 0);
	else
	if (!strcasecmp(arg, "two-justifiers"))
		S9xSetController(port, CTL_JUSTIFIER,  1, 0, 0, 0);
	else
	if (!strcasecmp(arg, "macsrifle"))
		S9xSetController(port, CTL_MACSRIFLE,  0, 0, 0, 0);
	else
	if (!strncasecmp(arg, "mp5:", 4) && ((arg[4] >= '1' && arg[4] <= '8') || arg[4] == 'n') &&
										((arg[5] >= '1' && arg[5] <= '8') || arg[5] == 'n') &&
										((arg[6] >= '1' && arg[6] <= '8') || arg[6] == 'n') &&
										((arg[7] >= '1' && arg[7] <= '8') || arg[7] == 'n') && arg[8] == '\0')
		S9xSetController(port, CTL_MP5, (arg[4] == 'n') ? -1 : arg[4] - '1',
										(arg[5] == 'n') ? -1 : arg[5] - '1',
										(arg[6] == 'n') ? -1 : arg[6] - '1',
										(arg[7] == 'n') ? -1 : arg[7] - '1');
	else
		return (false);

	return (true);
}


static bool try_load_config_file (const char *fname, ConfigFile &conf)
{
	FSTREAM	fp;

	fp = OPEN_FSTREAM(fname, "r");
	if (fp)
	{
		fprintf(stdout, "Reading config file %s.\n", fname);
		fStream fS(fp);
		conf.LoadFile(&fS);
        CLOSE_FSTREAM(fp);
		return (true);
	}

	return (false);
}

void S9xLoadConfigFiles (char **argv, int argc)
{
	static ConfigFile	conf; // static because some of its functions return pointers
	conf.Clear();

	for (int i = 0; i < argc - 1; i++)
		if (!strcasecmp(argv[i], "-conf"))
			try_load_config_file(argv[++i], conf);

	// Parse config file here

	// ROM

	Settings.ForceInterleaved2          =  conf.GetBool("ROM::Interleaved2",                   false);
	Settings.ForceInterleaveGD24        =  conf.GetBool("ROM::InterleaveGD24",                 false);
	Settings.NoPatch                    = !conf.GetBool("ROM::Patch",                          true);
	Settings.IgnorePatchChecksum        =  conf.GetBool("ROM::IgnorePatchChecksum",            false);

	Settings.ForceLoROM = conf.GetBool("ROM::LoROM", false);
	Settings.ForceHiROM = conf.GetBool("ROM::HiROM", false);
	if (Settings.ForceLoROM)
		Settings.ForceHiROM = false;

	Settings.ForcePAL   = conf.GetBool("ROM::PAL",  false);
	Settings.ForceNTSC  = conf.GetBool("ROM::NTSC", false);
	if (Settings.ForcePAL)
		Settings.ForceNTSC = false;

	if (conf.Exists("ROM::Header"))
	{
		Settings.ForceHeader = conf.GetBool("ROM::Header", false);
		Settings.ForceNoHeader = !Settings.ForceHeader;
	}

	if (conf.Exists("ROM::Interleaved"))
	{
		Settings.ForceInterleaved = conf.GetBool("ROM::Interleaved", false);
		Settings.ForceNotInterleaved = !Settings.ForceInterleaved;
	}

	rom_filename = conf.GetStringDup("ROM::Filename", NULL);
	Settings.InitialSnapshotFilename[0] = '\0';

	// Sound

	Settings.SoundSync                  =  conf.GetBool("Sound::Sync",                         false);
	Settings.SixteenBitSound            =  conf.GetBool("Sound::16BitSound",                   true);
	Settings.Stereo                     =  conf.GetBool("Sound::Stereo",                       true);
	Settings.ReverseStereo              =  conf.GetBool("Sound::ReverseStereo",                false);
	Settings.SoundPlaybackRate          =  conf.GetUInt("Sound::Rate",                         48000);
	Settings.SoundInputRate             =  conf.GetUInt("Sound::InputRate",                    31950);
	Settings.Mute                       =  conf.GetBool("Sound::Mute",                         false);
	Settings.DynamicRateControl         =  conf.GetBool("Sound::DynamicRateControl",           false);
	Settings.DynamicRateLimit           =  conf.GetInt ("Sound::DynamicRateLimit",             5);
	Settings.InterpolationMethod        =  conf.GetInt ("Sound::InterpolationMethod",          2);

	// Display

	Settings.SupportHiRes               =  conf.GetBool("Display::HiRes",                      true);
	Settings.Transparency               =  conf.GetBool("Display::Transparency",               true);
	Settings.DisableGraphicWindows      = !conf.GetBool("Display::GraphicWindows",             true);
	Settings.DisplayTime				=  conf.GetBool("Display::DisplayTime",                false);
	Settings.DisplayFrameRate           =  conf.GetBool("Display::DisplayFrameRate",           false);
	Settings.DisplayWatchedAddresses    =  conf.GetBool("Display::DisplayWatchedAddresses",    false);
	Settings.DisplayPressedKeys         =  conf.GetBool("Display::DisplayInput",               false);
	Settings.AutoDisplayMessages        =  conf.GetBool("Display::MessagesInImage",            true);
	Settings.InitialInfoStringTimeout   =  conf.GetInt ("Display::MessageDisplayTime",         120);
	Settings.BilinearFilter             =  conf.GetBool("Display::BilinearFilter",             false);

	// Settings

	Settings.TurboMode                  =  conf.GetBool("Settings::TurboMode",                 false);
	Settings.TurboSkipFrames            =  conf.GetUInt("Settings::TurboFrameSkip",            15);
	Settings.DontSaveOopsSnapshot       =  conf.GetBool("Settings::DontSaveOopsSnapshot",      false);
	Settings.AutoSaveDelay              =  conf.GetUInt("Settings::AutoSaveDelay",             0);

	if (conf.Exists("Settings::FrameTime"))
		Settings.FrameTimePAL = Settings.FrameTimeNTSC = conf.GetUInt("Settings::FrameTime", 16667);

	if (!strcasecmp(conf.GetString("Settings::FrameSkip", "Auto"), "Auto"))
		Settings.SkipFrames = AUTO_FRAMERATE;
	else
		Settings.SkipFrames = conf.GetUInt("Settings::FrameSkip", 0) + 1;

	// Controls

	Settings.MouseMaster                =  conf.GetBool("Controls::MouseMaster",               true);
	Settings.SuperScopeMaster           =  conf.GetBool("Controls::SuperscopeMaster",          true);
	Settings.JustifierMaster            =  conf.GetBool("Controls::JustifierMaster",           true);
	Settings.MacsRifleMaster            =  conf.GetBool("Controls::MacsRifleMaster",           true);
	Settings.MultiPlayer5Master         =  conf.GetBool("Controls::MP5Master",                 true);
	Settings.UpAndDown                  =  conf.GetBool("Controls::AllowLeftRight",            false);

	if (conf.Exists("Controls::Port1"))
		parse_controller_spec(0, conf.GetString("Controls::Port1"));
	if (conf.Exists("Controls::Port2"))
		parse_controller_spec(1, conf.GetString("Controls::Port2"));

	// Hack
    Settings.OverclockMode                  = conf.GetUInt("Hack::OverclockMode", 0);
    Settings.SeparateEchoBuffer             = conf.GetBool("Hack::SeparateEchoBuffer", false);
	Settings.DisableGameSpecificHacks       = !conf.GetBool("Hack::EnableGameSpecificHacks",       true);
	Settings.BlockInvalidVRAMAccessMaster   = !conf.GetBool("Hack::AllowInvalidVRAMAccess",        false);
	Settings.HDMATimingHack                 =  conf.GetInt ("Hack::HDMATiming",                    100);
	Settings.MaxSpriteTilesPerLine          =  conf.GetInt ("Hack::MaxSpriteTilesPerLine",         34);

	// Netplay

#ifdef NETPLAY_SUPPORT
	Settings.NetPlay = conf.GetBool("Netplay::Enable");

	Settings.Port = NP_DEFAULT_PORT;
	if (conf.Exists("Netplay::Port"))
		Settings.Port = -(int) conf.GetUInt("Netplay::Port");

	Settings.ServerName[0] = '\0';
	if (conf.Exists("Netplay::Server"))
		conf.GetString("Netplay::Server", Settings.ServerName, 128);
#endif

	// Debug

#ifdef DEBUGGER
	if (conf.GetBool("DEBUG::Debugger", false))
		CPU.Flags |= DEBUG_MODE_FLAG;

	if (conf.GetBool("DEBUG::Trace", false))
	{
		ENSURE_TRACE_OPEN(trace,"trace.log","wb")
		CPU.Flags |= TRACE_FLAG;
	}
	Settings.TraceSMP = FALSE;
#endif

	S9xParsePortConfig(conf, 1);
	S9xVerifyControllers();
}

void S9xUsage (void)
{
	/*                               12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
	exit(1);
}

void S9xParseArgsForCheats (char **argv, int argc)
{

}

char * S9xParseArgs (char **argv, int argc)
{
	for (int i = 1; i < argc; i++)
	{
		if (*argv[i] == '-')
		{
			if (!strcasecmp(argv[i], "-help"))
				S9xUsage();
			else

			// SOUND OPTIONS

			if (!strcasecmp(argv[i], "-soundsync"))
				Settings.SoundSync = TRUE;
			else if (!strcasecmp(argv[i], "-dynamicratecontrol"))
			{
				Settings.DynamicRateControl = TRUE;
				Settings.DynamicRateLimit = 5;
			}
			else
			if (!strcasecmp(argv[i], "-playbackrate"))
			{
				if (i + 1 < argc)
				{
					Settings.SoundPlaybackRate = atoi(argv[++i]);
					if (Settings.SoundPlaybackRate < 8192)
						Settings.SoundPlaybackRate = 8192;
				}
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-inputrate"))
			{
				if (i + 1 < argc)
				{
					Settings.SoundInputRate = atoi(argv[++i]);
					if (Settings.SoundInputRate < 31700)
						Settings.SoundInputRate = 31700;
					if (Settings.SoundInputRate > 32300)
						Settings.SoundInputRate = 32300;
				}
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-reversestereo"))
				Settings.ReverseStereo = TRUE;
			else
			if (!strcasecmp(argv[i], "-nostereo"))
				Settings.Stereo = FALSE;
			else
			if (!strcasecmp(argv[i], "-eightbit"))
				Settings.SixteenBitSound = FALSE;
			else
			if (!strcasecmp(argv[i], "-mute"))
				Settings.Mute = TRUE;
			else

			// DISPLAY OPTIONS

			if (!strcasecmp(argv[i], "-displaytime"))
				Settings.DisplayTime = TRUE;
			else
			if (!strcasecmp(argv[i], "-displayframerate"))
				Settings.DisplayFrameRate = TRUE;
			else
			if (!strcasecmp(argv[i], "-displaykeypress"))
				Settings.DisplayPressedKeys = TRUE;
			else
			if (!strcasecmp(argv[i], "-nohires"))
				Settings.SupportHiRes = FALSE;
			else
			if (!strcasecmp(argv[i], "-notransparency"))
				Settings.Transparency = FALSE;
			else
			if (!strcasecmp(argv[i], "-nowindows"))
				Settings.DisableGraphicWindows = TRUE;
			else

			// CONTROLLER OPTIONS

			if (!strcasecmp(argv[i], "-nomp5"))
				Settings.MultiPlayer5Master = FALSE;
			else
			if (!strcasecmp(argv[i], "-nomouse"))
				Settings.MouseMaster = FALSE;
			else
			if (!strcasecmp(argv[i], "-nosuperscope"))
				Settings.SuperScopeMaster = FALSE;
			else
			if (!strcasecmp(argv[i], "-nojustifier"))
				Settings.JustifierMaster = FALSE;
			else
			if (!strcasecmp(argv[i], "-nomacsrifle"))
				Settings.MacsRifleMaster = FALSE;
			else
			if (!strcasecmp(argv[i], "-port1") ||
				!strcasecmp(argv[i], "-port2"))
			{
				if (i + 1 < argc)
				{
					i++;
					if (!parse_controller_spec(argv[i - 1][5] - '1', argv[i]))
						S9xUsage();
				}
				else
					S9xUsage();
			}
			else

			// ROM OPTIONS

			if (!strcasecmp(argv[i], "-hirom"))
				Settings.ForceHiROM = TRUE;
			else
			if (!strcasecmp(argv[i], "-lorom"))
				Settings.ForceLoROM = TRUE;
			else
			if (!strcasecmp(argv[i], "-ntsc"))
				Settings.ForceNTSC = TRUE;
			else
			if (!strcasecmp(argv[i], "-pal"))
				Settings.ForcePAL = TRUE;
			else
			if (!strcasecmp(argv[i], "-nointerleave"))
				Settings.ForceNotInterleaved = TRUE;
			else
			if (!strcasecmp(argv[i], "-interleaved"))
				Settings.ForceInterleaved = TRUE;
			else
			if (!strcasecmp(argv[i], "-interleaved2"))
				Settings.ForceInterleaved2 = TRUE;
			else
			if (!strcasecmp(argv[i], "-interleavedgd24"))
				Settings.ForceInterleaveGD24 = TRUE;
			else
			if (!strcasecmp(argv[i], "-noheader"))
				Settings.ForceNoHeader = TRUE;
			else
			if (!strcasecmp(argv[i], "-header"))
				Settings.ForceHeader = TRUE;
			else

			// PATCH/CHEAT OPTIONS

			if (!strcasecmp(argv[i], "-nopatch"))
				Settings.NoPatch = TRUE;
			else
			// NETPLAY OPTIONS

		#ifdef NETPLAY_SUPPORT
			if (!strcasecmp(argv[i], "-net"))
				Settings.NetPlay = TRUE;
			else
			if (!strcasecmp(argv[i], "-port"))
			{
				if (i + 1 < argc)
					Settings.Port = -atoi(argv[++i]);
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-server"))
			{
				if (i + 1 < argc)
				{
					strncpy(Settings.ServerName, argv[++i], 127);
					Settings.ServerName[127] = 0;
				}
				else
					S9xUsage();
			}
			else
		#endif

			// HACKING OR DEBUGGING OPTIONS

		#ifdef DEBUGGER
			if (!strcasecmp(argv[i], "-debug"))
				CPU.Flags |= DEBUG_MODE_FLAG;
			else
			if (!strcasecmp(argv[i], "-trace"))
			{
				ENSURE_TRACE_OPEN(trace,"trace.log","wb")
				CPU.Flags |= TRACE_FLAG;
			}
			else
		#endif

			if (!strcasecmp(argv[i], "-hdmatiming"))
			{
				if (i + 1 < argc)
				{
					int	p = atoi(argv[++i]);
					if (p > 0 && p < 200)
						Settings.HDMATimingHack = p;
				}
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-invalidvramaccess"))
				Settings.BlockInvalidVRAMAccessMaster = FALSE;
			else

			// OTHER OPTIONS

			if (!strcasecmp(argv[i], "-frameskip"))
			{
				if (i + 1 < argc)
					Settings.SkipFrames = atoi(argv[++i]) + 1;
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-frametime"))
			{
				if (i + 1 < argc)
					Settings.FrameTimePAL = Settings.FrameTimeNTSC = atoi(argv[++i]);
				else
					S9xUsage();
			}
			else
			if (!strcasecmp(argv[i], "-upanddown"))
				Settings.UpAndDown = TRUE;
			else
			if (!strcasecmp(argv[i], "-conf"))
			{
				if (++i >= argc)
					S9xUsage();
				// Else do nothing, S9xLoadConfigFiles() handled it.
			}
			else
			if (!strcasecmp(argv[i], "-nostdconf"))
			{
				// Do nothing, S9xLoadConfigFiles() handled it.
			}
			else
				S9xParseArg(argv, i, argc);
		}
		else
			rom_filename = argv[i];
	}

	S9xVerifyControllers();

	return (rom_filename);
}
