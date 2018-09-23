#include <Magick++.h> 
#include <iostream>
#include <string>

using namespace std;
using namespace Magick;

#define IMAGE_RATIO 3
#define DEFAULT_RATION 1.0
#define WATERMARK_OFFSET 20

struct Arguments
{
	string source, watermark, destination;
	size_t scale = -1, quality = -1;
};

void add_watermark(Arguments& args, Image& image)
{
	if (args.watermark.size() == 0)
	{
		return;
	}

	size_t image_width = image.columns();
	size_t image_height = image.rows();

	Image watermark(args.watermark);
	size_t watermark_width = watermark.columns();
	size_t watermark_height = watermark.rows();

	if (image_width < watermark_width || image_height < watermark_height)
	{
		watermark_width = (int) (watermark_width / IMAGE_RATIO);
		watermark_height = (int) (watermark_height / IMAGE_RATIO);
	}
	else 
	{
		double width_ratio = image_width / IMAGE_RATIO / watermark_width;
		double height_ratio = image_height / IMAGE_RATIO / watermark_height;

		double ratio = min(width_ratio, height_ratio);

		if (width_ratio > 1 || height_ratio > 1) 
		{
			// if at least one ration is bigger than 1
			// then min ratio value mast be at least 1
			ratio = max(ratio, DEFAULT_RATION);
		}

		watermark_width = (int) (watermark_width * ratio);
		watermark_height = (int) (watermark_height * ratio);
	}

	watermark.scale(Geometry(watermark_width, watermark_height));

	image.composite(watermark, Magick::GravityType::SouthEastGravity, Magick::CompositeOperator::DissolveCompositeOp);
}

void scale_image(Arguments& args, Image& image)
{
	if (args.scale > 0)
	{
		image.scale(Geometry(args.scale, args.scale));
	}
}

void quality(Arguments& args, Image& image)
{
	if (args.quality > 0)
	{
		image.quality(args.quality);
	}
}

int main(int argc, char **argv)
{
	Arguments arguments;

	for (int i = 1; i < argc; ++i)
	{
		string argument(argv[i]);

		size_t equal = argument.find("=");
		string command = argument.substr(0, equal);
		string value = argument.substr(equal + 1);

		if (command == "source")
		{
			arguments.source = value;
		}
		else if (command == "watermark")
		{
			arguments.watermark = value;
		}
		else if (command == "destination")
		{
			arguments.destination = value;
		}
		else if (command == "quality")
		{
			int val = stoi(value);
			arguments.quality = val;
		}
		else if (command == "scale")
		{
			int val = stoi(value);
			arguments.scale = val;
		}
	}

	InitializeMagick(*argv);

	try {
		Image image(arguments.source);

		add_watermark(arguments, image);
		quality(arguments, image);
		scale_image(arguments, image);

		image.write(arguments.destination);
	}
	catch (Exception &error_)
	{
		cout << "Caught exception: " << error_.what() << endl;
		return 1;
	}
	return 0;
}