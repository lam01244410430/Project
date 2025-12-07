import React from 'react';
import { Upload, Button, message, Card, Typography } from 'antd';
import { UploadOutlined, DownloadOutlined } from '@ant-design/icons';
import Papa, { ParseResult } from 'papaparse';
import * as XLSX from 'xlsx';

const { Title } = Typography;

interface ScoreRow {
  studentId: number;
  subjectId: number;
  examId: number;
  score: number;
}

const ScoreEntry: React.FC = () => {
  const handleImport = (file: File): boolean => {
    Papa.parse<ScoreRow>(file, {
      header: false,
      skipEmptyLines: true,
      complete: async (result: ParseResult<ScoreRow>) => {
        const scores: ScoreRow[] = (result.data as any[])
          .map((row: any[]): ScoreRow => ({
            studentId: Number(row[0]),
            subjectId: Number(row[1]),
            examId: Number(row[2]),
            score: Number(row[3]),
          }))
          .filter((s: ScoreRow): s is ScoreRow => 
            !isNaN(s.studentId) && 
            !isNaN(s.subjectId) && 
            !isNaN(s.examId) && 
            !isNaN(s.score) && 
            s.score >= 0 && 
            s.score <= 150
          );

        if (scores.length === 0) {
          message.warning('No valid data found!');
          return;
        }

        try {
          const res = await fetch('http://localhost:8080/api/scores/batch', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(scores)
          }).then(r => r.json());
          message.success(`Imported ${res.success || 0} records!`);
        } catch {
          message.error('Import failed');
        }
      },
    });
    return false;
  };

  const exportTemplate = () => {
    const data = [["studentId", "subjectId", "examId", "score"], [101, 1, 1, 95.5]];
    const ws = XLSX.utils.aoa_to_sheet(data);
    const wb = XLSX.utils.book_new();
    XLSX.utils.book_append_sheet(wb, ws, "Template");
    XLSX.writeFile(wb, "Score_Template.xlsx");
  };

  return (
    <Card title={<Title level={3}>Score Entry</Title>}>
      <Upload beforeUpload={handleImport} accept=".csv,.xlsx" showUploadList={false}>
        <Button icon={<UploadOutlined />} type="primary">Import Excel</Button>
      </Upload>
      <Button icon={<DownloadOutlined />} onClick={exportTemplate} style={{ marginLeft: 16 }}>
        Download Template
      </Button>
    </Card>
  );
};

export default ScoreEntry;