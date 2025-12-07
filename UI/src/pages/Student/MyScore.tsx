import React, { useEffect, useState } from 'react';
import { Table, Card, Tag, Empty } from 'antd';
import { api } from '../../services/api';

const MyScores: React.FC = () => {
  const [scores, setScores] = useState<any[]>([]);
  
  const userStr = localStorage.getItem('user');
  const user = userStr ? JSON.parse(userStr) : null;

  useEffect(() => {
    const id = user?.userId || user?.id;

    if (id) {
      api.getMyScores(id)
        .then(res => setScores(res.data || []))
        .catch(err => console.error(err));
    }
  }, []);

  const columns = [
    { title: 'Subject', dataIndex: 'subject', key: 'subject' },
    { title: 'Exam', dataIndex: 'exam', key: 'exam' },
    { 
      title: 'Score', 
      dataIndex: 'score', 
      key: 'score', 
      render: (s: number) => (
        <Tag color={s >= 90 ? 'green' : s >= 60 ? 'blue' : 'red'}>
          {s}
        </Tag>
      ) 
    },
  ];

  if (!user) return <Empty description="Please login first" />;

  return (
    <Card title={`My Scores - ${user.realName || user.username}`}>
      <Table 
        dataSource={scores} 
        columns={columns} 
        rowKey={(record) => `${record.subject}_${record.exam}`} 
        pagination={false}
      />
    </Card>
  );
};

export default MyScores;