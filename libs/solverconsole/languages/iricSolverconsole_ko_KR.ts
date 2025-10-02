<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko_KR">
<context>
    <name>SolverConsoleWindow</name>
    <message>
        <source>&amp;Export solver console log...</source>
        <translation>솔버 콘솔 로그 내보내기 (&amp;E)...</translation>
    </message>
    <message>
        <source>Show Detail</source>
        <translation>세부 정보 표시</translation>
    </message>
    <message>
        <source>Warning</source>
        <translation>경고</translation>
    </message>
    <message>
        <source>The following problems found in the grid(s). Do you really want to run the solver with this grid?</source>
        <translation>그리드에서 다음 문제가 발견되었습니다. 이 그리드로 솔버를 실행하시겠습니까?</translation>
    </message>
    <message>
        <source>The simulation has result</source>
        <translation type="vanished">시뮬레이션 결과</translation>
    </message>
    <message>
        <source>Current simulation already has result data. When you run the solver, the current result data is discarded.</source>
        <translation type="vanished">현재 시뮬레이션에는 이미 결과 데이터가 있습니다. 솔버를 실행하면 현재 결과 데이터가 삭제됩니다.</translation>
    </message>
    <message>
        <source>Error occured. %1</source>
        <translation>문제 발생. %1</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>정보</translation>
    </message>
    <message>
        <source>This project is opened in post only mode. You can not run the solver.</source>
        <translation>이 프로젝트는 게시 전용 모드로 열립니다. 솔버를 실행할 수 없습니다.</translation>
    </message>
    <message>
        <source>We recommend that you save the project before starting the solver. Do you want to save?</source>
        <translation type="vanished">솔버를 시작하기 전에 프로젝트를 저장하는 것이 좋습니다. 저장하시겠습니까?</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>오류</translation>
    </message>
    <message>
        <source>Solver executable file %1 does not exists.</source>
        <translation>솔버 실행 파일 %1 존재하지 않습니다.</translation>
    </message>
    <message>
        <source>Confirm Solver Termination</source>
        <translation>솔버 종료 확인</translation>
    </message>
    <message>
        <source>Do you really want to kill the solver?</source>
        <translation>정말 해결사를 죽이고 싶습니까?</translation>
    </message>
    <message>
        <source>Solver Finished</source>
        <translation>솔버 완료</translation>
    </message>
    <message>
        <source>The solver finished calculation.</source>
        <translation type="vanished">솔버가 계산을 완료했습니다.</translation>
    </message>
    <message>
        <source>The solver finished abnormally.</source>
        <translation>솔버가 비정상적으로 완료되었습니다.</translation>
    </message>
    <message>
        <source>Solver Console</source>
        <translation>솔버 콘솔</translation>
    </message>
    <message>
        <source>running</source>
        <translation>달리기</translation>
    </message>
    <message>
        <source>stopped</source>
        <translation>멈췄다</translation>
    </message>
    <message>
        <source>Solver Console [%1] (%2)</source>
        <translation>솔버 콘솔 [ %1 ] (%2)</translation>
    </message>
    <message>
        <source>%1 seconds have passed, but the solver do not end. Do you want to kill the solver?</source>
        <translation>%1 초가 지났지만 솔버는 끝나지 않습니다. 해결사를 죽이고 싶습니까?</translation>
    </message>
    <message>
        <source>Background Color</source>
        <translation>배경색</translation>
    </message>
    <message>
        <source>Background Color Setting</source>
        <translation>배경색 설정</translation>
    </message>
    <message>
        <source>%1 does not exists. Please specify valid Python executable path in Preference dialog.</source>
        <translation>%1 존재하지 않습니다. 기본 설정 대화 상자에서 유효한 Python 실행 파일 경로를 지정하세요.</translation>
    </message>
    <message>
        <source>This project already has calculation result data. If you run the solver, it is discarded.</source>
        <translation type="vanished">이 프로젝트에는 이미 계산 결과 데이터가 있습니다. 솔버를 실행하면 삭제됩니다.</translation>
    </message>
    <message>
        <source>The solver finished calculation. Do you want to save this project?</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SolverConsoleWindowProjectDataItem</name>
    <message>
        <source>Select File to Export</source>
        <translation>내보낼 파일 선택</translation>
    </message>
    <message>
        <source>Text file (*.txt)</source>
        <translation>텍스트 파일 (*.txt)</translation>
    </message>
    <message>
        <source>Exporting solver console log...</source>
        <translation>솔버 콘솔 로그를 내보내는 중...</translation>
    </message>
    <message>
        <source>Solver console log is successfully exported to %1.</source>
        <translation>솔버 콘솔 로그를 다음으로 성공적으로 내보냈습니다. %1.</translation>
    </message>
    <message>
        <source>Fail</source>
        <translation>불합격</translation>
    </message>
    <message>
        <source>Exporting solver console log failed.</source>
        <translation>솔버 콘솔 로그 내보내기에 실패했습니다.</translation>
    </message>
</context>
<context>
    <name>SolverConsoleWindow_MessageDialog</name>
    <message>
        <source>Warning</source>
        <translation>경고</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
&lt;p&gt;Please make sure:&lt;/p&gt;
&lt;ul&gt;
&lt;li&gt;If calculation result already exists, it is deleted.&lt;/li&gt;
&lt;li&gt;The calculation condition and input grid that you can see on pre-processing window is written to Case1.cgn.&lt;/li&gt;
&lt;li&gt;The solver will read the data from Case1.cgn and start running.&lt;/li&gt;
&lt;/ul&gt;
&lt;p&gt;Are you sure it is OK?&lt;/p&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt; &lt;p&gt;다음을 확인하세요:&lt;/p&gt; &lt;ul&gt; &lt;li&gt;계산 결과가 이미 존재하는 경우 삭제됩니다.&lt;/li&gt; &lt;li&gt;계산 조건 및 입력 그리드 전처리 창에서 볼 수 있는 내용이 Case1.cgn에 기록됩니다.&lt;/li&gt; &lt;li&gt;솔버가 Case1.cgn에서 데이터를 읽고 실행을 시작합니다.&lt;/li&gt; &lt;/ul&gt; &lt;p&gt;당신은 정말 괜찮나요?&lt;/p&gt; &lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Please make sure:&lt;/p&gt;&lt;ul style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px;&quot;&gt;&lt;li style=&quot;margin-top:8px;&quot;&gt;If calculation result already exists, it will be deleted.&lt;/li&gt;&lt;li style=&quot;margin-top:8px&quot;&gt;The project will be saved.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;Are you sure it is OK?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;다음을 확인하세요:&lt;/p&gt;&lt;ul style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; &quot;&gt;&lt;li style=&quot;margin-top:8px;&quot;&gt;계산 결과가 이미 존재할 경우 삭제됩니다.&lt;/li&gt;&lt;li style=&quot;margin-top:8px&quot;&gt;프로젝트가 저장됩니다.&lt;/li&gt; li&gt;&lt;/ul&gt;&lt;p&gt;정말 괜찮나요?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
</context>
</TS>
