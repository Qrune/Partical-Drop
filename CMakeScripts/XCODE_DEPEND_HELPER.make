# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.ParticleDrop.Debug:
/Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/Debug/ParticleDrop:
	/bin/rm -f /Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/Debug/ParticleDrop


PostBuild.ParticleDrop.Release:
/Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/Release/ParticleDrop:
	/bin/rm -f /Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/Release/ParticleDrop


PostBuild.ParticleDrop.MinSizeRel:
/Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/MinSizeRel/ParticleDrop:
	/bin/rm -f /Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/MinSizeRel/ParticleDrop


PostBuild.ParticleDrop.RelWithDebInfo:
/Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/RelWithDebInfo/ParticleDrop:
	/bin/rm -f /Users/yanggao/Documents/Git\ Hub/Partical-Drop/build/RelWithDebInfo/ParticleDrop




# For each target create a dummy ruleso the target does not have to exist
