#include "Weapons/BOImpactFeedbackData.h"

const FBOImpactFeedback& UBOImpactFeedbackData::FindFeedback(EPhysicalSurface SurfaceType) const
{
	for (const FBOImpactSurfaceFeedback& Entry : SurfaceFeedback)
	{
		if (Entry.SurfaceType == SurfaceType)
		{
			return Entry.Feedback;
		}
	}

	return DefaultFeedback;
}
