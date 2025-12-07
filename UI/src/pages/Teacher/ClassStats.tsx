import React from 'react';
import ReactECharts from 'echarts-for-react';
import { Card } from 'antd';

const ClassStats: React.FC = () => {
  const stats = [
    { subject: 'Mathematics', avg: 88.5, passRate: 95 },
    { subject: 'Physics', avg: 82.3, passRate: 88 },
    { subject: 'Chemistry', avg: 85.1, passRate: 91 },
    { subject: 'English', avg: 90.2, passRate: 97 },
    { subject: 'Biology', avg: 87.0, passRate: 93 },
  ];

  const option = {
    title: { text: 'Class Performance Statistics', left: 'center' },
    tooltip: { trigger: 'axis' },
    legend: { data: ['Average Score', 'Pass Rate (%)'], top: 30 },
    xAxis: { type: 'category', data: stats.map(s => s.subject) },
    yAxis: [
      { type: 'value', name: 'Score', min: 0, max: 100 },
      { type: 'value', name: 'Pass Rate (%)', min: 0, max: 100, position: 'right' }
    ],
    series: [
      {
        name: 'Average Score',
        type: 'bar',
        data: stats.map(s => s.avg),
        itemStyle: { color: '#5470c6' },
        emphasis: { focus: 'series' }
      },
      {
        name: 'Pass Rate (%)',
        type: 'line',
        yAxisIndex: 1,
        data: stats.map(s => s.passRate),
        itemStyle: { color: '#91cc75' },
        symbol: 'circle',
        symbolSize: 8
      }
    ]
  };

  return (
    <Card title="Class Statistics Dashboard">
      <ReactECharts option={option} style={{ height: 500, width: '100%' }} />
    </Card>
  );
};

export default ClassStats;