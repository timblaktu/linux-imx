// SPDX-License-Identifier: GPL-2.0
// ak4554.c
//
// Copyright (C) 2013 Renesas Solutions Corp.
// Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>

#include <linux/module.h>
#include <sound/soc.h>

/*
 * ak4558 is very simple DA/AD converter
 *
 * CAUTION
 *
 * ak4558 playback format is SND_SOC_DAIFMT_RIGHT_J,
 * and,   capture  format is SND_SOC_DAIFMT_LEFT_J
 * on same bit clock, LR clock.
 * But, this driver doesn't have snd_soc_dai_ops :: set_fmt
 *
 * CPU/Codec DAI image
 *
 * CPU-DAI1 (plaback only fmt = RIGHT_J) --+-- ak4558
 *					   |
 * CPU-DAI2 (capture only fmt = LEFT_J) ---+
 */

static const struct snd_soc_dapm_widget ak4558_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("AINL"),
SND_SOC_DAPM_INPUT("AINR"),

SND_SOC_DAPM_OUTPUT("AOUTL"),
SND_SOC_DAPM_OUTPUT("AOUTR"),
};

static const struct snd_soc_dapm_route ak4558_dapm_routes[] = {
	{ "Capture", NULL, "AINL" },
	{ "Capture", NULL, "AINR" },

	{ "AOUTL", NULL, "Playback" },
	{ "AOUTR", NULL, "Playback" },
};

static struct snd_soc_dai_driver ak4558_dai = {
	.name = "ak4558-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE,
	},
	.symmetric_rates = 1,
};

static const struct snd_soc_component_driver soc_component_dev_ak4558 = {
	.dapm_widgets		= ak4558_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(ak4558_dapm_widgets),
	.dapm_routes		= ak4558_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(ak4558_dapm_routes),
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static int ak4558_soc_probe(struct platform_device *pdev)
{
	return devm_snd_soc_register_component(&pdev->dev,
				      &soc_component_dev_ak4558,
				      &ak4558_dai, 1);
}

static const struct of_device_id ak4558_of_match[] = {
	{ .compatible = "asahi-kasei,ak4558" },
	{},
};
MODULE_DEVICE_TABLE(of, ak4558_of_match);

static struct platform_driver ak4558_driver = {
	.driver = {
		.name = "ak4558-adc-dac",
		.of_match_table = ak4558_of_match,
	},
	.probe	= ak4558_soc_probe,
};
module_platform_driver(ak4558_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("SoC AK4558 driver");
MODULE_AUTHOR("Arthur Chow <arthur.chow@xilica.com>");
