#pragma once

namespace WPC
{
	class ChartExporter
	{
	protected:
		const Chart* pChart;

	public:
		ChartExporter(const Chart* pChart);

		virtual bool SaveChartToFile(const std::string& filePath, std::string& errorText) = 0;
	};
}