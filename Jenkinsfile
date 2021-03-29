pipeline {
    agent none
    triggers {cron '@daily'}
    options {
        buildDiscarder(logRotator(numToKeepStr: '5', daysToKeepStr: '30'))
        timestamps()
        timeout(unit: 'HOURS', time: 2)
    }
    
    stages {
        stage ('Parallel') {
            parallel {
                stage ('Linux') {
                    agent { label 'docker-amt' }
                    stages {
                        stage ('Cloning Repository') {
                            steps {
                                script {
                                    scmCheckout {
                                        clean = true
                                    }
                                }
                            }
                        }
                    }
                }
                stage ('Windows') {
                    agent { label 'openamt-win' }
                    stages {
                        stage ('Cloning Repository') {
                            steps {
                                script {
                                    scmCheckout {
                                        clean = true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        stage ('Static Code Scan - Protex') {
            agent { label 'docker-amt' }
            steps {
                script {
                    staticCodeScan {
                        // generic
                        scanners             = ['protex']
                        scannerType          = ['c','c++']

                        protexProjectName    = 'OpenAMT - RPC'
                        // internal, do not change
                        protexBuildName      = 'rrs-generic-protex-build'
                    }
                }
            }
        }

        stage ('Parallel Builds') {
            parallel {
                stage ('Linux') {
                    agent { label 'docker-amt' }
                    stages {
                        stage('Build') {
                            agent {
                                docker {
                                    image 'ubuntu:18.04'
                                    reuseNode true
                                }
                            }
                            steps {
                                sh './scripts/jenkins-pre-build.sh'
                                sh './scripts/jenkins-build.sh'
                            }
                        }
                        stage ('Archive') {
                            steps {
                                archiveArtifacts allowEmptyArchive: true, artifacts: 'build/rpc', caseSensitive: false, onlyIfSuccessful: true
                            }
                        }
                        
                    }
                }
                stage ('Windows') {
                    agent { label 'openamt-win' }
                    stages{
                        stage ('Build') {
                            steps {
                                bat 'scripts\\jenkins-pre-build.cmd'
                                bat 'scripts\\jenkins-build.cmd'
                                // prepare stash for the binary scan
                                stash includes: "**/*.exe", name: 'rpc-app'
                            }
                        }
                        stage ('Archive') {
                            steps {
                                archiveArtifacts allowEmptyArchive: true, artifacts: 'build\\Release\\rpc.exe', caseSensitive: false, onlyIfSuccessful: true
                            }
                        }
                    }
                }
            }
        }
        stage ('Parallel Scans') {
            parallel {
                stage ('Static Code Scan Linux') {
                    agent { label 'docker-amt' }
                    steps {
                        script {
                            staticCodeScan {
                                // generic
                                scanners             = ['bdba','klocwork']
                                scannerType          = 'c++'

                                protecodeGroup          = '25'
                                protecodeScanName       = 'rpc-zip'
                                protecodeDirectory      = './build/rpc'
                                
                                klockworkPreBuildScript = './scripts/jenkins-pre-build.sh'
                                klockworkBuildCommand = './scripts/jenkins-build.sh'
                                klockworkProjectName  = 'Panther Point Creek'
                                klockworkIgnoreCompileErrors = true
                            }
                        }
                    }
                }
                stage ('Static Code Scan Windows') {
                    stages {
                        stage ('Static Code Scan Windows - Klockwork') {
                            agent { label 'openamt-win' }
                            steps {
                                script {
                                    staticCodeScan {
                                        // generic
                                        scanners             = ['klocwork']
                                        scannerType          = 'c++'
                                        
                                        klockworkPreBuildScript = 'scripts\\jenkins-pre-build.cmd'
                                        klockworkBuildCommand = 'scripts\\jenkins-build.cmd'
                                        klockworkProjectName  = 'Panther Point Creek'
                                        klockworkIgnoreCompileErrors = true
                                    }
                                }
                            }
                        }
                        stage ('Static Code Scan Windows - BDBA') {
                            agent { label 'docker-amt' }
                            steps {
                                script {
                                    sh "mkdir -p bdbaScanDir"
                                    dir("bdbaScanDir") {
                                        unstash 'rpc-app'
                                    }
                                    staticCodeScan {
                                        // generic
                                        scanners             = ['bdba']
                                        scannerType          = 'c++'
                                        
                                        protecodeGroup          = '25'
                                        protecodeScanName       = 'rpc-zip'
                                        protecodeDirectory      = 'bdbaScanDir'
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}