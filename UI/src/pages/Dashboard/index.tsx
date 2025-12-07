import React from 'react'; // Thêm dòng này nếu chưa có
import { Card, Row, Col, Statistic, Empty, Spin } from 'antd';
import { useRequest } from 'ahooks';
import { api } from '../../services/api';
import BarChart from '../../components/Charts/BarChart';
import RadarChart from '../../components/Charts/RadarChart';

// QUAN TRỌNG: Component phải được export default
export default function Dashboard() {
  // Đảm bảo api.getStats và api.getClasses là các hàm trả về Promise
  const { data: statsData, loading: statsLoading } = useRequest(api.getStats);
  
  // Lỗi có thể ở đây nếu api.getClasses chưa được định nghĩa trong api.ts
  // Hãy kiểm tra file api.ts xem có hàm getClasses không
  const { data: classData, loading: classLoading } = useRequest(api.getClasses); 

  const subjectStats = statsData?.data || [];
  const classes = classData?.data || [];

  const totalStudents = classes.reduce((sum: number, c: any) => sum + (c.studentCount || 0), 0);
  const schoolAverage = subjectStats.length > 0
    ? subjectStats.reduce((sum: number, s: any) => sum + (s.avg || 0), 0) / subjectStats.length
    : 0;
  const passRate = subjectStats.length > 0
    ? subjectStats.reduce((sum: number, s: any) => sum + (s.passRate || 0), 0) / subjectStats.length
    : 0;

  const classPerformance = classes.map((c: any) => ({
    name: c.name || 'Unknown',
    value: c.averageScore || 0
  }));

  if (statsLoading || classLoading) {
    return (
      <div style={{ textAlign: 'center', padding: '100px 0' }}>
        <Spin size="large" tip="Loading data from server..." />
      </div>
    );
  }

  const hasData = subjectStats.length > 0;

  return (
    <Row gutter={[16, 16]}>
      {/* Radar Chart */}
      <Col span={12}>
        <Card title="School Performance Radar Chart" bordered={false}>
          {hasData ? (
            <RadarChart data={subjectStats} title="Subject Performance Overview" />
          ) : (
            <Empty 
              image={Empty.PRESENTED_IMAGE_SIMPLE} 
              description="No score data available yet"
            />
          )}
        </Card>
      </Col>

      {/* Statistics + Bar Chart */}
      <Col span={12}>
        <Card title="School Statistics" bordered={false}>
          <Row gutter={[16, 16]}>
            <Col span={12}>
              <Statistic title="Total Classes" value={classes.length} />
            </Col>
            <Col span={12}>
              <Statistic title="Total Students" value={totalStudents || 'N/A'} />
            </Col>
            <Col span={12}>
              <Statistic 
                title="School Average Score" 
                value={hasData ? schoolAverage.toFixed(1) : 'N/A'} 
                suffix={hasData ? '/ 100' : ''}
                precision={1}
              />
            </Col>
            <Col span={12}>
              <Statistic 
                title="Overall Pass Rate" 
                value={hasData ? passRate.toFixed(1) : 'N/A'} 
                suffix={hasData ? '%' : ''}
                valueStyle={{ color: hasData && passRate >= 90 ? '#3f8600' : '#cf1322' }}
              />
            </Col>
          </Row>

          <div style={{ marginTop: 32 }}>
            <Card title="Class Performance Ranking" size="small">
              {classes.length > 0 ? (
                <BarChart 
                  data={classPerformance} 
                  title="Average Score by Class" 
                  color="#91cc75" 
                />
              ) : (
                <Empty description="No class data available" />
              )}
            </Card>
          </div>
        </Card>
      </Col>
    </Row>
  );
}