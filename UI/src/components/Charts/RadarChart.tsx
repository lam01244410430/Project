import React from 'react';
import ReactECharts from 'echarts-for-react';

interface SubjectData {
  subject: string;
  avg: number;
}

interface RadarChartProps {
  data: SubjectData[];
  title?: string;
}

const RadarChart: React.FC<RadarChartProps> = ({ 
  data, 
  title = 'Subject Performance Radar' 
}) => {
  const indicator = data.map(d => ({ name: d.subject, max: 100 }));
  const values = data.map(d => d.avg || 0);

  const option = {
    title: {
      text: title,
      left: 'center',
      textStyle: { fontSize: 16, fontWeight: 'bold' }
    },
    tooltip: { trigger: 'item' },
    radar: {
      indicator,
      radius: '65%',
      name: { textStyle: { color: '#333' } }
    },
    series: [{
      type: 'radar',
      data: [{
        value: values,
        areaStyle: { opacity: 0.1 },
        lineStyle: { width: 2 },
        itemStyle: { color: '#5470c6' }
      }]
    }]
  };

  return (
    <ReactECharts 
      option={option} 
      style={{ height: '450px', width: '100%' }} 
      notMerge={true}
      lazyUpdate={true}
    />
  );
};

export default RadarChart;