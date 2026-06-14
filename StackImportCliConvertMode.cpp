#include "StackImportCli.h"

#include "stackimport_logging.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace stackimport::cli {

struct ConvertOutput {
	std::string output_path;
	FILE* outfile = nullptr;
	size_t bytes_written = 0;
};

int STACKIMPORT_CALL convert_payload_to_file(
	const stackimport_resource_payload* /*payload*/,
	const void* data,
	size_t size,
	void* user_data)
{
	auto* output = static_cast<ConvertOutput*>(user_data);
	if(!output->outfile)
		return 0;
	size_t written = fwrite(data, 1, size, output->outfile);
	if(written != size)
		return 0;
	output->bytes_written += written;
	return 1;
}

std::string default_output_path(const std::string& input_path, const std::string& type)
{
	std::string output_path = input_path + "." + type;
	std::replace(output_path.begin(), output_path.end(), '/', '_');
	return output_path;
}

int convert_single_file(
	const std::string& input_path,
	const std::string& output_path,
	const Options& options)
{
	ConvertOutput output;
	output.output_path = output_path;

	output.outfile = fopen(output.output_path.c_str(), "wb");
	if(!output.outfile)
	{
		stackimport_quill_diagnosticf("Error: Could not open '%s' for writing.\n", output.output_path.c_str());
		return 5;
	}

	std::vector<uint8_t> buf;
	if(!read_entire_file(input_path, buf))
	{
		fclose(output.outfile);
		stackimport_quill_diagnosticf("Error: Failed to read '%s'.\n", input_path.c_str());
		return 5;
	}

	stackimport_resource_conversion_options conversion = {};
	stackimport_resource_conversion_options_init(&conversion);
	std::memcpy(conversion.type, options.resource_type.c_str(), 4);
	conversion.id = options.resource_id;
	conversion.data = buf.data();
	conversion.data_size = buf.size();
	conversion.resource_payload_flags = options.resource_converted ?
		STACKIMPORT_RESOURCE_PAYLOADS_CONVERTED :
		STACKIMPORT_RESOURCE_PAYLOADS_NATIVE;
	conversion.resource_payload = convert_payload_to_file;
	conversion.resource_user_data = &output;

	stackimport_status status = stackimport_convert_resource(&conversion);
	fclose(output.outfile);

	if(status != STACKIMPORT_STATUS_OK)
	{
		stackimport_quill_diagnosticf("Error: Conversion failed: %s.\n", stackimport_status_string(status));
		return 5;
	}

	stackimport_quill_logf(STACKIMPORT_MESSAGE_INFO, "Converted %s #%d -> %s (%zu bytes)\n",
		options.resource_type.c_str(),
		options.resource_id,
		output.output_path.c_str(),
		output.bytes_written);
	return 0;
}

int run_convert_mode(const Options& options)
{
	if(options.resource_type.size() != 4)
	{
		stackimport_quill_diagnosticf("Error: --type must be exactly 4 bytes (e.g., PICT, snd , ICON).\n");
		return 3;
	}

	const std::string& input_path = options.input_path;
	std::string output_path = options.output_path;
	if(output_path.empty())
		output_path = default_output_path(input_path, options.resource_type);

	return convert_single_file(input_path, output_path, options);
}

} // namespace stackimport::cli