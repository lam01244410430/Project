import React from 'react';
import ReactECharts from 'echarts-for-react';

interface DataPoint {
  name: string;
  value: number;
}

interface BarChartProps {
  data: DataPoint[];
  title?: string;
  color?: string;
}

const BarChart: React.FC<BarChartProps> = ({ 
  data, 
  title = 'Bar Chart', 
  color = '#5470c6' 
}) => {
  const option = {
    title: {
      text: title,
      left: 'center',
      textStyle: { fontSize: 16, fontWeight: 'bold' }
    },
    tooltip: { trigger: 'axis' },
    grid: { top: 60, bottom: 40 },
    xAxis: {
      type: 'category',
      data: data.map(d => d.name),
      axisLabel: { rotate: 30 }
    },
    yAxis: { type: 'value' },
    series: [{
      data: data.map(d => ({
        value: d.value,
        itemStyle: { color }
      })),
      type: 'bar',
      barWidth: '60%',
      emphasis: { focus: 'series' }
    }]
  };

  return (
    <ReactECharts 
      option={option} 
      style={{ height: '400px', width: '100%' }} 
      notMerge={true}
      lazyUpdate={true}
    />
  );
};

export default BarChart;